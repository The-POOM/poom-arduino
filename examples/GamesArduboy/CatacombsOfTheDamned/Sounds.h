#pragma once
#include <Poom.h>
#include <stdint.h>
#include "Defines.h"

class Sounds
{
public:
	static const PoomNote Attack[];
	static const PoomNote Kill[];
	static const PoomNote Hit[];
	static const PoomNote PlayerDeath[];
	static const PoomNote SpotPlayer[];
	static const PoomNote Shoot[];
	static const PoomNote Pickup[];
	static const PoomNote Ouch[];
};
