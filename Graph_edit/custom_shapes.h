#pragma once

#include <windows.h>

class CustomShape abstract
{
protected:
	int x, y;

public:
	static int penWidth, penStyle;
	static COLORREF penColor;

	CustomShape(int, int);
	int getX();
	int getY();
	void setX(int);
	void setY(int);
	virtual void draw(HDC&, int, int) = 0;
};

class CustomLine : public CustomShape
{
public:
	CustomLine(int, int);
	virtual void draw(HDC&, int, int) override;
};

class CustomPencil : public CustomLine
{
public:
	CustomPencil(int, int);
	virtual void draw(HDC&, int, int) override;
};

class CustomRubber : public CustomPencil
{
public:
	static int rubberWidth;
	static COLORREF rubberColor;

	CustomRubber(int, int);
	void draw(HDC&, int, int) override;
	void ellipse(HDC&, int, int, int);
};

class CustomRectangle : public CustomShape
{
public:
	CustomRectangle(int, int);
	void draw(HDC&, int, int) override;
};

class CustomEllipse: public CustomShape
{
public:
	CustomEllipse(int, int);
	void draw(HDC&, int, int) override;
};
