#pragma once
#include "math.h"

uint Ceiling(uint number, uint divider)
{
	return (number + divider - 1) / divider;
}
