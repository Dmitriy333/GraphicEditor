#include "custom_rectangle.h"

CustomRectangle::CustomRectangle(int newX, int newY) : CustomShape(newX, newY){}

void CustomRectangle::draw(HDC &dc, int newX, int newY)
{
	Rectangle(dc, x, y, newX, newY);
}