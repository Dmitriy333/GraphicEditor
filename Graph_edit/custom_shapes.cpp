#include "custom_shapes.h"

// Shape
int CustomShape::penWidth = 1;
COLORREF CustomShape::penColor = RGB(0,0,0);

CustomShape::CustomShape(int newX, int newY) : x(newX), y(newY){}

int CustomShape::getX()
{
	return x;
}

int CustomShape::getY()
{
	return y;
}

void CustomShape::setX(int newX)
{
	x = newX;
}

void CustomShape::setY(int newY)
{
	y = newY;
}

// Line
CustomLine::CustomLine(int newX, int newY) : CustomShape(newX, newY){}

void CustomLine::draw(HDC &dc, int newX, int newY)
{
	MoveToEx(dc, x, y, NULL);
	LineTo(dc, newX, newY);
}

//Pencil
CustomPencil::CustomPencil(int newX, int newY) : CustomLine(newX, newY){}

void CustomPencil::draw(HDC &dc, int newX, int newY)
{
	__super::draw(dc, newX, newY);
	x = newX;
	y = newY;
}


//Rubber
int CustomRubber::rubberWidth = 20;
COLORREF CustomRubber::rubberColor = RGB(255, 255, 255);

CustomRubber::CustomRubber(int newX, int newY) : CustomPencil(newX, newY){}

void CustomRubber::draw(HDC &dc, int x, int y)
{
	__super::draw(dc, x, y);
}

void CustomRubber::ellipse(HDC &dc, int x, int y, int width)
{
	int side = width / 2;
	Ellipse(dc, x - side, y - side, x + side, y + side);
}

// Rectangle
CustomRectangle::CustomRectangle(int newX, int newY) : CustomShape(newX, newY){}

void CustomRectangle::draw(HDC &dc, int newX, int newY)
{
	Rectangle(dc, x, y, newX, newY);
}

// Circle
CustomEllipse::CustomEllipse(int newX, int newY) : CustomShape(newX, newY){}

void CustomEllipse::draw(HDC &dc, int newX, int newY)
{
	Ellipse(dc, x, y, newX, newY);
}
