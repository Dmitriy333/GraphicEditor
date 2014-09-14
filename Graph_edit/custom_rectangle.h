#pragma once

#include "custom_shape.h"

class CustomRectangle : public CustomShape
{
public:
	CustomRectangle(int, int);
	void draw(HDC&, int, int);
};