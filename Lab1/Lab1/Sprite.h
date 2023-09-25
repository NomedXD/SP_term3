#pragma once
#include <Windows.h>

struct mousePositionStruct
{
	float 
		top, 
		bottom, 
		left, 
		right, 
		mouseX, 
		mouseY;
};

class Sprite
{
	//  онтексты отображени€ устройства
	HDC device;
	HDC bufferDevice;
	// Ѕитовые растровые изображени€
	HBITMAP hScreen;
	HBITMAP oldBmp;
	// –абочее окно(размеры)
	RECT currentWindow;
	// »нформаци€, котоар€, может быть использовала дл€ окрашивани€ рабочей области окна принадлежащей этому приложению
	PAINTSTRUCT PStruct;
	mousePositionStruct points;
	// ќпредел€ет тип, ширину, высоту, формат цвета и битовые значени€ растрового изображени€
	BITMAP bm;

	HDC spriteDevice;
	HBITMAP image;

public:
	Sprite(HWND hwnd, RECT workSpace);
	void drawRect(HWND hWnd, float X1, float Y1, float X2, float Y2);
	void drawSprite(HWND hWnd, float X, float Y);
	mousePositionStruct* getMousePositionStruct(float width);
};