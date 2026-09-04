#!/usr/bin/env python3
"""Split legacy interleaved page-packed sprite data into image and mask arrays."""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path


def parse_initializer(path: Path) -> list[int]:
    source = path.read_text(encoding="utf-8")
    source = re.sub(r"/\*.*?\*/", "", source, flags=re.DOTALL)
    source = re.sub(r"//.*", "", source)

    opening = source.find("{")
    closing = source.rfind("}")
    if opening < 0 or closing <= opening:
        raise ValueError("input must contain one C/C++ array initializer")

    initializer = source[opening + 1 : closing]
    tokens = re.findall(r"(?<![\w.])(?:0[xX][0-9a-fA-F]+|\d+)(?:[uUlL]+)?", initializer)
    values = [int(token.rstrip("uUlL"), 0) for token in tokens]

    if not values:
        raise ValueError("input array contains no byte values")
    if any(value > 0xFF for value in values):
        raise ValueError("input array contains a value outside the byte range")

    return values


def format_array(name: str, values: list[int]) -> str:
    lines = []
    for offset in range(0, len(values), 12):
        row = ", ".join(f"0x{value:02X}" for value in values[offset : offset + 12])
        lines.append(f"    {row}")
    body = ",\n".join(lines)
    return f"static const uint8_t {name}[] PROGMEM = {{\n{body}\n}};"


def identifier(value: str) -> str:
    if not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", value):
        raise argparse.ArgumentTypeError("name must be a valid C/C++ identifier")
    return value


def positive_byte(value: str) -> int:
    number = int(value)
    if number < 1 or number > 255:
        raise argparse.ArgumentTypeError("value must be between 1 and 255")
    return number


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description=(
            "Convert alternating page-packed image/mask bytes into separate "
            "arrays for drawPageBitmapMasked()."
        )
    )
    parser.add_argument("input", type=Path, help="C/C++ file containing one byte array")
    parser.add_argument("--width", required=True, type=positive_byte, help="frame width in pixels")
    parser.add_argument("--height", required=True, type=positive_byte, help="frame height in pixels")
    parser.add_argument("--name", default="sprite", type=identifier, help="base output array name")
    parser.add_argument(
        "--order",
        choices=("image-mask", "mask-image"),
        default="image-mask",
        help="order of each interleaved byte pair (default: image-mask)",
    )
    parser.add_argument("-o", "--output", type=Path, help="write output to this header instead of stdout")
    return parser


def main() -> int:
    parser = build_parser()
    args = parser.parse_args()

    try:
        values = parse_initializer(args.input)
    except (OSError, ValueError) as error:
        parser.error(str(error))

    frame_size = args.width * ((args.height + 7) // 8)
    interleaved_frame_size = frame_size * 2
    if len(values) % interleaved_frame_size != 0:
        parser.error(
            f"found {len(values)} bytes; expected a multiple of "
            f"{interleaved_frame_size} for {args.width}x{args.height} frames"
        )

    first = values[0::2]
    second = values[1::2]
    images, masks = (first, second) if args.order == "image-mask" else (second, first)
    frame_count = len(values) // interleaved_frame_size
    guard = f"{args.name.upper()}_PAGE_SPRITE_H"

    output = "\n".join(
        (
            f"#ifndef {guard}",
            f"#define {guard}",
            "",
            "#include <Arduino.h>",
            "",
            f"// {frame_count} frame(s), {args.width}x{args.height} pixels each.",
            format_array(f"{args.name}Frames", images),
            "",
            format_array(f"{args.name}Masks", masks),
            "",
            f"#endif  // {guard}",
            "",
        )
    )

    if args.output:
        args.output.write_text(output, encoding="utf-8")
    else:
        sys.stdout.write(output)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
