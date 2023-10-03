#include "Sprite.h"

// Создание картинки
Sprite::Sprite(HWND hwnd, RECT currentWindow)
{
	// Извлекает дискриптор контекста устройства для экрана(структура данных, описывающая устройство отображения информации)
	device = GetDC(hwnd);
	// Создает контекст устройства памяти (DC), совместимый с указанным устройством(без этого изображение моргает :((()
	bufferDevice = CreateCompatibleDC(device);
	this->currentWindow = currentWindow;
	// Cоздает растровое изображение, совместимое с устройством, которое связано с указанным контекстом устройства.
	hScreen = CreateCompatibleBitmap(device, currentWindow.right, currentWindow.bottom);
	// Выбирает объект в указанном контексте устройства (DC). Новый объект заменяет предыдущий объект того же типа.
	oldBmp = (HBITMAP)SelectObject(bufferDevice, hScreen);
	// Загрузка изображения
	image = (HBITMAP)LoadImage(0, L"images/bitmap.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
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
// Рисование картинки
void Sprite::drawSprite(HWND hWnd, float X, float Y)
{
	BeginPaint(hWnd, &PStruct);
	// Рисует указанный прямоугольник с помощью кисти, выбранной в данный момент в контексте указанного устройства
	PatBlt(bufferDevice, 0, 0, currentWindow.right, currentWindow.bottom, WHITENESS);
	// Перенос контекста совместимого устройства в его исходный контестк(с которым совместим)
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