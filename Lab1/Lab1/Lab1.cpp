#include "Sprite.h"
#include <Windows.h>
#include <cmath>
#include <chrono>
#include <thread>

// Указатель на нашу картинку
Sprite* picutrePointer;
CenterCorrection* centerPoints;
// Размер нашего экрана
RECT currentWindow = { 0, 0, 700, 450 };
// Координаты и размеры картинки(прямоугольинка)
float coordX = 0.0f;
float coordY = 0.0f;
float width = 50.0f;
float height = 60.0f;
float pictureWidth = 50.0f;
float pictureHeight = 50.0f;
// Скорость картинки
float pictureSpeed = 0.5f;
float wheelPictureSpeed = 15;
// Для столкновения с краями экрана
const int speepingCollisionTime = 100;
const int borderCollisionBounce = 25;
const float borderCollisionBounceStep = 0.25;
bool bounceTop = false;
bool bounceBottom = false;
bool bounceRight = false;
bool bounceLeft = false;

void checkMouseMovementKeyPress(HWND& winHandle);
void checkMovementKeysPress();
void checkIfBouncedNow();
void checkCurrentWindowCollisions(HWND& winHandle);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Словили ли сообщение о прокрутке колесика и нажатие шифта
	if (uMsg == WM_MOUSEWHEEL && GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT)
	{
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
			coordX += wheelPictureSpeed;
		else
			coordX -= wheelPictureSpeed;
	}
	else if (uMsg == WM_MOUSEWHEEL && GET_KEYSTATE_WPARAM(wParam) != MK_SHIFT)
	{
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
			coordY += wheelPictureSpeed;
		else
			coordY -= wheelPictureSpeed;
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
		// picutrePointer->drawRect(hwnd, X, Y, X+width, Y+height);
		picutrePointer->drawSprite(hwnd, coordX, coordY);
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR cmdLine, int nCmdShow)
{
	// Содержит сведения о классе окна
	WNDCLASSEX windowClass;
	// Содержит информацию сообщения из очереди
	MSG message;


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
	coordX = (currentWindow.right - currentWindow.left) / 2.f;
	coordY = (currentWindow.bottom - currentWindow.top) / 2.f;

	message.message = WM_NULL;
	// Создание прямоугольника и инициализация указателя
	picutrePointer = new Sprite(winHandle, currentWindow);
	if (!picutrePointer) return -1;
	// Инициализация точек корректировки центра картинки(для передвижения по мышке)
	centerPoints = picutrePointer->initializeCenterCorrectionPoints(width, height);
	ShowWindow(winHandle, nCmdShow);
	// Пока мы не вышли из приложения
	while (message.message != WM_QUIT)
	{
		GetClientRect(winHandle, &currentWindow);

		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&message);
		}
		checkMouseMovementKeyPress(winHandle);

		// Проверки на нажатие стрелочек и изменеине координат картинки
		checkMovementKeysPress();

		// Проверка, сталкивалась картинка с краем, чтобы отбросить ее с задержкой
		checkIfBouncedNow();

		// Проверки на столкновение с краями экрана
		checkCurrentWindowCollisions(winHandle);

		// Перерисовка окна(происходит не сразу наверное)
		InvalidateRect(winHandle, NULL, FALSE);
	}
	delete picutrePointer;

	return 0;
}

void checkMouseMovementKeyPress(HWND& winHandle) {
	if (GetAsyncKeyState(VK_LBUTTON))
	{
		// Точка-координаты курсора
		POINT mouseCoords;
		// Получение координат курсора
		GetCursorPos(&mouseCoords);
		// Перевод координат мышки из экранных в клиентские
		ScreenToClient(winHandle, &mouseCoords);
		// Корректировка координат мышки(чтобы спрайт двигался точно к центру)
		mouseCoords.x += centerPoints->x; mouseCoords.y += centerPoints->y;
		// Расстояние евклида от мышки до центра картинки, чтобы она двигалась плавно, а не моментально
		float euclidDistance = sqrt((mouseCoords.x - coordX) * (mouseCoords.x - coordX) + (mouseCoords.y - coordY) * (mouseCoords.y - coordY));
		// Передвижение каждой координаты по-отдельности
		coordX += pictureSpeed * (mouseCoords.x - coordX) / euclidDistance;
		coordY += pictureSpeed * (mouseCoords.y - coordY) / euclidDistance;
		if (euclidDistance < 0.5f)
		{
			coordX = mouseCoords.x;
			coordY = mouseCoords.y;
		}
	}
}

void checkMovementKeysPress() {
	if (GetAsyncKeyState(VK_UP))
		coordY -= pictureSpeed;
	if (GetAsyncKeyState(VK_DOWN))
		coordY += pictureSpeed;
	if (GetAsyncKeyState(VK_LEFT))
		coordX -= pictureSpeed;
	if (GetAsyncKeyState(VK_RIGHT))
		coordX += pictureSpeed;
}

void checkIfBouncedNow() {
	if (bounceTop) {
		coordY += borderCollisionBounceStep;
		std::this_thread::sleep_for(std::chrono::nanoseconds(speepingCollisionTime));
	}

	if (coordY > currentWindow.top + borderCollisionBounce) {
		bounceTop = false;
	}

	if (bounceRight) {
		coordX -= borderCollisionBounceStep;
		std::this_thread::sleep_for(std::chrono::nanoseconds(speepingCollisionTime));
	}

	if (coordX + pictureWidth < currentWindow.right - borderCollisionBounce) {
		bounceRight = false;
	}

	if (bounceBottom) {
		coordY -= borderCollisionBounceStep;
		std::this_thread::sleep_for(std::chrono::nanoseconds(speepingCollisionTime));
	}

	if (coordY + pictureHeight < currentWindow.bottom - borderCollisionBounce) {
		bounceBottom = false;
	}

	if (bounceLeft) {
		coordX += borderCollisionBounceStep;
		std::this_thread::sleep_for(std::chrono::nanoseconds(speepingCollisionTime));
	}

	if (coordX > currentWindow.left + borderCollisionBounce) {
		bounceLeft = false;
	}
}

void checkCurrentWindowCollisions(HWND& winHandle) {
	if (coordY <= currentWindow.top) {
		bounceTop = true;
		coordY = currentWindow.top;
		InvalidateRect(winHandle, NULL, FALSE);
	}
	if (coordX + pictureWidth >= currentWindow.right) {
		bounceRight = true;
		coordX = currentWindow.right - pictureWidth;
		InvalidateRect(winHandle, NULL, FALSE);
	}
	if (coordY + pictureHeight >= currentWindow.bottom) {
		bounceBottom = true;
		coordY = currentWindow.bottom - pictureHeight;
		InvalidateRect(winHandle, NULL, FALSE);
	}
	if (coordX <= currentWindow.left) {
		bounceLeft = true;
		coordX = currentWindow.left;
		InvalidateRect(winHandle, NULL, FALSE);
	}
}