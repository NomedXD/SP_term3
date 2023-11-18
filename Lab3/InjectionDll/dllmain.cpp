//#include "pch.h"
#include "dllfunctionreplace.h"
#include <Windows.h>
#include <cstdio>
#include <iostream>

/*
Необязательная точка входа в библиотеку динамической компоновки (DLL). 
Когда система запускает или завершает процесс или поток, она вызывает функцию 
точки входа для каждой загруженной библиотеки DLL с помощью первого потока процесса. 
Система также вызывает функцию точки входа для библиотеки DLL при ее загрузке или выгрузке 
с помощью функций LoadLibrary и FreeLibrary
*/
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    // Получение PID процесса 
    DWORD pid = GetCurrentProcessId();
    // Код причины, указывающий, почему вызывается функция точки входа DLL
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}