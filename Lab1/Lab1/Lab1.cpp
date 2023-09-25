#include "Sprite.h"
#include <Windows.h>
#include <cmath>
#include <chrono>
#include <thread>

// Указатель на нашу картинку
Sprite* gfx;
// Доп точки для мышки
mousePositionStruct* mousePositionPoints;
// Размер нашего экрана
RECT currentWindow = { 0, 0, 700, 450 };
// Координаты и размеры картинки(прямоугольинка)
float X = 0.0f;
float Y = 0.0f;
float width = 50.0f;
float height = 60.0f;
float pictureWidth = 50.0f;
float pictureHeight = 50.0f;
// Скорость картинки
float SPEED = 0.5f;
float WHEELPRESSSPEED = 15;

void checkMovementKeysPress();
void checkMouseKeyPress(POINT& mouseCoords, HWND& winHandle, float& distance);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Словили ли сообщшение о прокрутке колесика и нажатие шифта
	if (uMsg == WM_MOUSEWHEEL && GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT)
	{
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
			X += WHEELPRESSSPEED;
		else
			X -= WHEELPRESSSPEED;
	}
	else if (uMsg == WM_MOUSEWHEEL && GET_KEYSTATE_WPARAM(wParam) != MK_SHIFT)
	{
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
			Y += WHEELPRESSSPEED;
		else
			Y -= WHEELPRESSSPEED;
	}
	// Отправляется при уничтожении окна.
	if (uMsg == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}
	// Сообщение WM_PAINT отправляется, когда система или другое приложение отправляет запрос на закрашивание части окна приложения. Сообщение отправляется при вызове функции UpdateWindow или RedrawWindow или функцией DispatchMessage
	if (uMsg == WM_PAINT)
	{
		gfx->drawRect(hwnd, X, Y, X+width, Y+height);
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR cmdLine, int nCmdShow)
{
	// Содержит сведения о классе окна
	WNDCLASSEX windowClass;
	// Содержит информацию сообщения из очереди
	MSG message;
	// Точка-координаты курсора
	POINT mouseCoords;
	float distance = 0.0f;

	// Заполняет блок памяти нулями
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));
	// Размер данной структуры (в байтах)
	windowClass.cbSize = sizeof(WNDCLASSEX);
	// Дескриптор фоновой кисти класса(может быть значением цвета)
	windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	// Дескриптор экземпляра , содержащего процедуру окна для класса
	windowClass.hInstance = hInstance;
	// Указатель на процедуру окна ????????
	windowClass.lpfnWndProc = WindowProc;
	// Указатель на символьную строку с пустым завершением, указывающую имя ресурса
	windowClass.lpszClassName = L"MainWindow";
	// Стили классов
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	// Создает перекрывающееся всплывающее окно или дочернее окно с расширенным стилем окна
	RegisterClassEx(&windowClass);
	// Вычисляет требуемый размер прямоугольника окна на основе желаемого размера прямоугольника
	AdjustWindowRectEx(&currentWindow, WS_OVERLAPPEDWINDOW, NULL, WS_EX_OVERLAPPEDWINDOW);
	// Дискриптор окна(обязателен для переадчи при рисовании чего-либо)
	HWND winHandle = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"MainWindow", L"Moving Figure", WS_OVERLAPPEDWINDOW,
		100, 100, currentWindow.right - currentWindow.left, currentWindow.bottom - currentWindow.top, NULL, NULL, hInstance, 0);

	if (!winHandle) return -1;

	// Начальное положение прямоугольника
	X = (currentWindow.right - currentWindow.left) / 2.f;
	Y = (currentWindow.bottom - currentWindow.top) / 2.f;

	message.message = WM_NULL;
	// Создание прямоугольника и инициализация указателя
	gfx = new Sprite(winHandle, currentWindow);
	if (!gfx) return -1;
	mousePositionPoints = gfx->getMousePositionStruct(width);

	ShowWindow(winHandle, nCmdShow);
	// Пока мы не вышли из приложения
	while (message.message != WM_QUIT)
	{
		GetClientRect(winHandle, &currentWindow);

		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&message);
		}
		// Проверка нажатия на кнопку мыши
		checkMouseKeyPress(mouseCoords, winHandle, distance);

		// Проверки на нажатие стрелочек и изменеине координат картинки
		checkMovementKeysPress();

		// Перерисовка окна(происходит не сразу наверное)
		InvalidateRect(winHandle, NULL, FALSE);
	}
	delete gfx;

	return 0;
}

void checkMovementKeysPress() {
	if (GetAsyncKeyState(VK_UP))
		Y -= SPEED;
	if (GetAsyncKeyState(VK_DOWN))
		Y += SPEED;
	if (GetAsyncKeyState(VK_LEFT))
		X -= SPEED;
	if (GetAsyncKeyState(VK_RIGHT))
		X += SPEED;
}

void checkMouseKeyPress(POINT& mouseCoords, HWND& winHandle, float& distance) {
	if (GetAsyncKeyState(VK_LBUTTON))
	{
		// Получаем координаты курсора
		GetCursorPos(&mouseCoords);
		// преобразует координаты точки в системе координат экрана (начало координат левый верхний угол экрана) 
		// в систему координат клиентской области компонента (начало координат левый верхний угол клиентской области).
		ScreenToClient(winHandle, &mouseCoords);
		// Отнимаем половину ширины/ длины картинки, чтобы ее центр двигался к курсору, а не левый край
		mouseCoords.x += mousePositionPoints->mouseX; mouseCoords.y += mousePositionPoints->mouseY;
		// Считаем Евклидово расстояние до курсора мыши
		distance = sqrt((mouseCoords.x - X) * (mouseCoords.x - X) + (mouseCoords.y - Y) * (mouseCoords.y - Y));
		// добавляем неЕвклидово расстояние к координатам и делим на реальное расстояние для равномерности движ-я
		X += SPEED * (mouseCoords.x - X) / distance;
		Y += SPEED * (mouseCoords.y - Y) / distance;
		// Доводим до центра, если осталось совсем мало расстояния
		if (distance < 0.5f)
		{
			X = mouseCoords.x;
			Y = mouseCoords.y;
		}
	}
}