#pragma once

#include <Arduino.h>
#include <Poom.h>

#ifndef WIDTH
#define WIDTH 128
#endif

#ifndef HEIGHT
#define HEIGHT 64
#endif

#ifndef BLACK
#define BLACK 0
#endif

#ifndef WHITE
#define WHITE 1
#endif

#ifndef INVERT
#define INVERT 2
#endif

#ifndef CLEAR_BUFFER
#define CLEAR_BUFFER true
#endif

#ifndef ARDUBOY_NO_USB
#define ARDUBOY_NO_USB
#endif

#ifndef LEFT_BUTTON
#define LEFT_BUTTON PoomButtonLeft
#define RIGHT_BUTTON PoomButtonRight
#define UP_BUTTON PoomButtonUp
#define DOWN_BUTTON PoomButtonDown
#define A_BUTTON PoomButtonA
#define B_BUTTON PoomButtonB
#endif

#ifndef RED_LED
#define RED_LED 0
#define GREEN_LED 1
#define BLUE_LED 2
#endif

#if !defined(__uint24)
#define __uint24 uint32_t
#endif

using uint24_t = __uint24;

struct Point
{
    int16_t x;
    int16_t y;
};

struct Rect
{
    int16_t x;
    int16_t y;
    uint8_t width;
    uint8_t height;
};
