#include "Sprite.h"

// �������� ��������
Sprite::Sprite(HWND hwnd, RECT currentWindow)
{
	// ��������� ���������� ��������� ���������� ��� ������(��������� ������, ����������� ���������� ����������� ����������)
	device = GetDC(hwnd);
	// ������� �������� ���������� ������ (DC), ����������� � ��������� �����������(��� ����� ����������� ������� :((()
	bufferDevice = CreateCompatibleDC(device);
	this->currentWindow = currentWindow;
	// C������ ��������� �����������, ����������� � �����������, ������� ������� � ��������� ���������� ����������.
	hScreen = CreateCompatibleBitmap(device, currentWindow.right, currentWindow.bottom);
	// �������� ������ � ��������� ��������� ���������� (DC). ����� ������ �������� ���������� ������ ���� �� ����.
	oldBmp = (HBITMAP)SelectObject(bufferDevice, hScreen);
	// �������� �����������
	image = (HBITMAP)LoadImage(0, L"images/bitmap.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	// ���������� ���, ������, ������, ������ ����� � ������� �������� ���������� �����������
	GetObject(image, sizeof(BITMAP), &bm);
	spriteDevice = CreateCompatibleDC(device);
	SelectObject(spriteDevice, image);
}

void Sprite::drawRect(HWND hWnd, float X1, float Y1, float X2, float Y2)
{
	BeginPaint(hWnd, &PStruct);
	// ������ ��������� ������������� � ������� �����, ��������� � ������ ������ � ��������� ���������� ����������
	PatBlt(bufferDevice, 0, 0, currentWindow.right, currentWindow.bottom, WHITENESS);
	Rectangle(bufferDevice, (int)X1, (int)Y1, (int)X2, (int)Y2);
	BitBlt(device, 0, 0, currentWindow.right, currentWindow.bottom, bufferDevice, 0, 0, SRCCOPY);

	EndPaint(hWnd, &PStruct);
}
// ��������� ��������
void Sprite::drawSprite(HWND hWnd, float X, float Y)
{
	BeginPaint(hWnd, &PStruct);
	// ������ ��������� ������������� � ������� �����, ��������� � ������ ������ � ��������� ���������� ����������
	PatBlt(bufferDevice, 0, 0, currentWindow.right, currentWindow.bottom, WHITENESS);
	// ������� ��������� ������������ ���������� � ��� �������� ��������(� ������� ���������)
	BitBlt(bufferDevice, (int)X, (int)Y, bm.bmWidth, bm.bmHeight, spriteDevice, 0, 0, SRCCOPY);
	BitBlt(device, 0, 0, currentWindow.right, currentWindow.bottom, bufferDevice, 0, 0, SRCCOPY);

	EndPaint(hWnd, &PStruct);
}

CenterCorrection* Sprite::initializeCenterCorrectionPoints(float width, float height)
{
	centerCorrectionPoints.x = -width / 2;
	centerCorrectionPoints.y = -height / 2;
	return &centerCorrectionPoints;
}