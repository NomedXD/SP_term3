#include "Sprite.h"

Sprite::Sprite(HWND hwnd, RECT currentWindow)
{
	device = GetDC(hwnd);
	bufferDevice = CreateCompatibleDC(device);
	this->currentWindow = currentWindow;
	// Cоздает изображение, совместимое с устройством, которое связано с указанным контекстом устройства.
	hScreen = CreateCompatibleBitmap(device, currentWindow.right, currentWindow.bottom);
	// Выбирает объект в указанном контексте устройства (DC). Новый объект заменяет предыдущий объект того же типа.
	oldBmp = (HBITMAP)SelectObject(bufferDevice, hScreen);
	// Определяет тип, ширину, высоту, формат цвета и битовые значения растрового изображения
	GetObject(image, sizeof(BITMAP), &bm);
	spriteDevice = CreateCompatibleDC(device);
	SelectObject(spriteDevice, image);
}

void Sprite::drawRect(HWND hWnd, float X1, float Y1, float X2, float Y2)
{
	BeginPaint(hWnd, &PStruct);
	// Рисует указанный прямоугольник с помощью кисти, выбранной в данный момент в контексте указанного устройства
	PatBlt(bufferDevice, 0, 0, currentWindow.right, currentWindow.bottom, WHITENESS);
	Rectangle(bufferDevice, (int)X1, (int)Y1, (int)X2, (int)Y2);
	BitBlt(device, 0, 0, currentWindow.right, currentWindow.bottom, bufferDevice, 0, 0, SRCCOPY);

	EndPaint(hWnd, &PStruct);
}

// Вычисление доп. точек
mousePositionStruct* Sprite::getMousePositionStruct(float width)
{
	points.bottom = width;
	points.left = 0;
	points.right = width;
	points.top = 0;
	points.mouseX = -width / 2;
	points.mouseY = -width / 2;
	return &points;
}