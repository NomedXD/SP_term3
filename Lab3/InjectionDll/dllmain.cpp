//#include "pch.h"
#include "dllfunctionreplace.h"
#include <Windows.h>
#include <cstdio>
#include <iostream>

/*
�������������� ����� ����� � ���������� ������������ ���������� (DLL). 
����� ������� ��������� ��� ��������� ������� ��� �����, ��� �������� ������� 
����� ����� ��� ������ ����������� ���������� DLL � ������� ������� ������ ��������. 
������� ����� �������� ������� ����� ����� ��� ���������� DLL ��� �� �������� ��� �������� 
� ������� ������� LoadLibrary � FreeLibrary
*/
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    // ��������� PID �������� 
    DWORD pid = GetCurrentProcessId();
    // ��� �������, �����������, ������ ���������� ������� ����� ����� DLL
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