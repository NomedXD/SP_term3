#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <vector>
#include <tlhelp32.h>
#include <locale>

void staticImport(DWORD pid);
void dynamicImport(DWORD pid);

// Функция замены строки в памяти
const char FUNCTION_NAME[] = "ReplaceStringInMemory";
// Строки и путь к библиоткеке
#define RESULT_STR "Chumack"
#define DLL_PATH "InjectionDll.dll"
#define ORIGINAL_STRING "Witcher"

#pragma comment(lib, "E:\\SP_term3\\Lab3\\x64\\Debug\\InjectionDll.lib")

extern "C" _declspec(dllimport) void __stdcall ReplaceStringInMemory(DWORD PID, const char* srcString, const char* destString);
typedef int(__cdecl* TReplaceMemoryFunc)(DWORD PID, const char*, const char*);

// Структура для удобства
struct Params {
    DWORD pid;
    // Исходная строка
    char originalStr[10];
    // Итоговая строка
    char resultStr[10];
};


int main()
{
    setlocale(LC_ALL, "Russian");
    // Получение PID текущего процесса
    DWORD pid = GetCurrentProcessId();
    // Создание в памяти исходной строки
    char data1[] = ORIGINAL_STRING;
    std::cout << "PID текущего процесса: " << pid << std::endl;

    DWORD injectionPID = 0;
    std::cout << "Статический импорт библиотеки здесь" << std::endl;
    staticImport(pid);
    std::cout << "Здесь меняется строка: " << ORIGINAL_STRING << " на строку: " << data1 << std::endl;
    std::cout << "Динамический импорт библиотеки здесь" << std::endl;
    dynamicImport(pid);
    std::cout << "Здесь меняется строка: " << RESULT_STR << " на строку: " << data1 << std::endl;
    return 0;
}

void staticImport(DWORD pid)
{
    ReplaceStringInMemory(pid, ORIGINAL_STRING, RESULT_STR);
}

void dynamicImport(DWORD pid)
{
    // Загружает указанный модуль(библиотеки из curnal x32) в адресное пространство вызывающего процесса
    HMODULE hDll = LoadLibraryA(DLL_PATH);
    if (hDll == NULL)
        return;
    // Поиск функции замены строк и ее вызов
    TReplaceMemoryFunc func = (TReplaceMemoryFunc)GetProcAddress(hDll, FUNCTION_NAME); 
    func(pid, RESULT_STR, ORIGINAL_STRING);
    // Освобождает загруженный модуль библиотеки динамических ссылок
    FreeLibrary(hDll);
}