#pragma once
#include <Windows.h>

struct CenterCorrection
{
	float x, y;
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
	CenterCorrection centerCorrectionPoints;
	// ���������� ���, ������, ������, ������ ����� � ������� �������� ���������� �����������
	BITMAP bm;

	HDC spriteDevice;
	HBITMAP image;

public:
	Sprite(HWND hwnd, RECT workSpace);
	void drawRect(HWND hWnd, float X1, float Y1, float X2, float Y2);
	void drawSprite(HWND hWnd, float X, float Y);
	CenterCorrection* initializeCenterCorrectionPoints(float width, float height);
};