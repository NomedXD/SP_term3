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
	// ��������� ����������� ����������
	HDC device;
	HDC bufferDevice;
	// ������� ��������� �����������
	HBITMAP hScreen;
	HBITMAP oldBmp;
	// ������� ����(�������)
	RECT currentWindow;
	// ����������, �������, ����� ���� ������������ ��� ����������� ������� ������� ���� ������������� ����� ����������
	PAINTSTRUCT PStruct;
	mousePositionStruct points;
	// ���������� ���, ������, ������, ������ ����� � ������� �������� ���������� �����������
	BITMAP bm;

	HDC spriteDevice;
	HBITMAP image;

public:
	Sprite(HWND hwnd, RECT workSpace);
	void drawRect(HWND hWnd, float X1, float Y1, float X2, float Y2);
	void drawSprite(HWND hWnd, float X, float Y);
	mousePositionStruct* getMousePositionStruct(float width);
};