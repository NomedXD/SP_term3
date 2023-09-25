#include "Sprite.h"
#include <Windows.h>
#include <cmath>
#include <chrono>
#include <thread>

// ��������� �� ���� ��������
Sprite* gfx;
// ��� ����� ��� �����
mousePositionStruct* mousePositionPoints;
// ������ ������ ������
RECT currentWindow = { 0, 0, 700, 450 };
// ���������� � ������� ��������(��������������)
float X = 0.0f;
float Y = 0.0f;
float width = 50.0f;
float height = 60.0f;
// �������� ��������
float SPEED = 0.5f;

void checkMovementKeysPress();

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// ������������ ��� ����������� ����.
	if (uMsg == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}
	// ��������� WM_PAINT ������������, ����� ������� ��� ������ ���������� ���������� ������ �� ������������ ����� ���� ����������. ��������� ������������ ��� ������ ������� UpdateWindow ��� RedrawWindow ��� �������� DispatchMessage
	if (uMsg == WM_PAINT)
	{
		gfx->drawRect(hwnd, X, Y, X+width, Y+height);
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR cmdLine, int nCmdShow)
{
	// �������� �������� � ������ ����
	WNDCLASSEX windowClass;
	// �������� ���������� ��������� �� �������
	MSG message;
	// �����-���������� �������
	POINT mouseCoords;
	float distance = 0.0f;

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
	X = (currentWindow.right - currentWindow.left) / 2.f;
	Y = (currentWindow.bottom - currentWindow.top) / 2.f;

	message.message = WM_NULL;
	// �������� �������������� � ������������� ���������
	gfx = new Sprite(winHandle, currentWindow);
	if (!gfx) return -1;

	ShowWindow(winHandle, nCmdShow);
	// ���� �� �� ����� �� ����������
	while (message.message != WM_QUIT)
	{
		GetClientRect(winHandle, &currentWindow);

		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&message);
		}

		// �������� �� ������� ��������� � ��������� ��������� ��������
		checkMovementKeysPress();

		// ����������� ����(���������� �� ����� ��������)
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