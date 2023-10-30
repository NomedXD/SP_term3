#include <Windows.h>
#include <iostream>
#include <string>

#include "App.hpp"

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE,  LPSTR szCmdLine, int nCmdShow)
{
    App app{"Lab2/resources/text.txt"};
    return app.run();
}
