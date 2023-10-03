#include "Sprite.h"
#include <Windows.h>
#include <cmath>
#include <chrono>
#include <thread>

// ��������� �� ���� ��������
Sprite* picutrePointer;
CenterCorrection* centerPoints;
// ������ ������ ������
RECT currentWindow = { 0, 0, 700, 450 };
// ���������� � ������� ��������(��������������)
float coordX = 0.0f;
float coordY = 0.0f;
float width = 50.0f;
float height = 60.0f;
float pictureWidth = 50.0f;
float pictureHeight = 50.0f;
// �������� ��������
float pictureSpeed = 0.5f;
float wheelPictureSpeed = 15;
// ��� ������������ � ������ ������
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
	// ������� �� ��������� � ��������� �������� � ������� �����
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
	// ������������ ��� ����������� ����.
	if (uMsg == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}
	// ��������� WM_PAINT ������������, ����� ������� ��� ������ ���������� ���������� ������ �� ������������ ����� ���� ����������. ��������� ������������ ��� ������ ������� UpdateWindow ��� RedrawWindow ��� �������� DispatchMessage
	if (uMsg == WM_PAINT)
	{
		// picutrePointer->drawRect(hwnd, X, Y, X+width, Y+height);
		picutrePointer->drawSprite(hwnd, coordX, coordY);
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR cmdLine, int nCmdShow)
{
	// �������� �������� � ������ ����
	WNDCLASSEX windowClass;
	// �������� ���������� ��������� �� �������
	MSG message;


	// ��������� ���� ������ ������
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));
	// ������ ������ ��������� (� ������)
	windowClass.cbSize = sizeof(WNDCLASSEX);
	// ���������� ������� ����� ������(����� ���� ��������� �����)
	windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	// ���������� ���������� , ����������� ��������� ���� ��� ������
	windowClass.hInstance = hInstance;
	// ��������� �� ��������� ���� ????????
	windowClass.lpfnWndProc = WindowProc;
	// ��������� �� ���������� ������ � ������ �����������, ����������� ��� �������
	windowClass.lpszClassName = L"MainWindow";
	// ����� �������
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	// ������� ��������������� ����������� ���� ��� �������� ���� � ����������� ������ ����
	RegisterClassEx(&windowClass);
	// ��������� ��������� ������ �������������� ���� �� ������ ��������� ������� ��������������
	AdjustWindowRectEx(&currentWindow, WS_OVERLAPPEDWINDOW, NULL, WS_EX_OVERLAPPEDWINDOW);
	// ���������� ����(���������� ��� �������� ��� ��������� ����-����)
	HWND winHandle = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"MainWindow", L"Moving Figure", WS_OVERLAPPEDWINDOW,
		100, 100, currentWindow.right - currentWindow.left, currentWindow.bottom - currentWindow.top, NULL, NULL, hInstance, 0);

	if (!winHandle) return -1;

	// ��������� ��������� ��������������
	coordX = (currentWindow.right - currentWindow.left) / 2.f;
	coordY = (currentWindow.bottom - currentWindow.top) / 2.f;

	message.message = WM_NULL;
	// �������� �������������� � ������������� ���������
	picutrePointer = new Sprite(winHandle, currentWindow);
	if (!picutrePointer) return -1;
	// ������������� ����� ������������� ������ ��������(��� ������������ �� �����)
	centerPoints = picutrePointer->initializeCenterCorrectionPoints(width, height);
	ShowWindow(winHandle, nCmdShow);
	// ���� �� �� ����� �� ����������
	while (message.message != WM_QUIT)
	{
		GetClientRect(winHandle, &currentWindow);

		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&message);
		}
		checkMouseMovementKeyPress(winHandle);

		// �������� �� ������� ��������� � ��������� ��������� ��������
		checkMovementKeysPress();

		// ��������, ������������ �������� � �����, ����� ��������� �� � ���������
		checkIfBouncedNow();

		// �������� �� ������������ � ������ ������
		checkCurrentWindowCollisions(winHandle);

		// ����������� ����(���������� �� ����� ��������)
		InvalidateRect(winHandle, NULL, FALSE);
	}
	delete picutrePointer;

	return 0;
}

void checkMouseMovementKeyPress(HWND& winHandle) {
	if (GetAsyncKeyState(VK_LBUTTON))
	{
		// �����-���������� �������
		POINT mouseCoords;
		// ��������� ��������� �������
		GetCursorPos(&mouseCoords);
		// ������� ��������� ����� �� �������� � ����������
		ScreenToClient(winHandle, &mouseCoords);
		// ������������� ��������� �����(����� ������ �������� ����� � ������)
		mouseCoords.x += centerPoints->x; mouseCoords.y += centerPoints->y;
		// ���������� ������� �� ����� �� ������ ��������, ����� ��� ��������� ������, � �� �����������
		float euclidDistance = sqrt((mouseCoords.x - coordX) * (mouseCoords.x - coordX) + (mouseCoords.y - coordY) * (mouseCoords.y - coordY));
		// ������������ ������ ���������� ��-�����������
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