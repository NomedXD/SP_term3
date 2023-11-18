#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <vector>
#include <tlhelp32.h>
#include <locale>

void staticImport(DWORD pid);
void dynamicImport(DWORD pid);
void inject(DWORD pid);

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
    std::cout << "Для внедрения в процесс main библиоткеи введите PID: " << std::endl;
    std::cin >> injectionPID;
    inject(injectionPID);
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

void inject(DWORD pid) {
    // Получение доступа к процессу main, в который проводится инджект, через PID
    HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (process == NULL) {
        std::cout << "Нет такого процесса с указанным PID - " << pid << std::endl;
        exit(1);
    }

    // Поиск адреса функции LoadLibraryA, которая загружает dll библиотеку, в модуле kernel32
#pragma warning(suppress : 6387)
    LPVOID loadLibraryPointer = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");
    if (loadLibraryPointer == NULL) {
        printf("Не найдена загружающая функция LoadLibraryA");
        exit(-1);
    }

    // Выделение динамической памяти под название подключаемой библиотеки
    LPVOID allocForStrings = VirtualAllocEx(process, NULL, 1024, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (allocForStrings == NULL) {
        std::cout << "Память под название библиотеки не выделилась" << std::endl;
        exit(-1);
    }
    // Запись этого названия в память
    if (WriteProcessMemory(process, allocForStrings, DLL_PATH, strlen(DLL_PATH) + 1, NULL) == 0) {
        std::cout << "Запись имени библиотеки в память завершилась с ошибкой " << std::endl;
        exit(-1);
    }

    // Выделение памяти под саму библиотеку
    LPVOID allocParamPtr = VirtualAllocEx(process, NULL, strlen(DLL_PATH) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (allocParamPtr == NULL) {
        std::cout << "Невозможно выделить память под библиотеку" << std::endl;
        exit(-1);
    }
    if (WriteProcessMemory(process, allocParamPtr, &allocForStrings, sizeof(LPVOID), NULL) == 0) {
        std::cout << "Имя библиотеки не записалось в память" << std::endl;
        exit(-1);
    }

    // Создание удаленного потока, loadLibraryPointer - указатель на первую функцию потока, которая и загрузит dll
    HANDLE thread = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibraryPointer, allocForStrings, 0, NULL);
    if (thread == NULL) {
        std::cout << "Поток для инджекта dll не создался" << std::endl;
        exit(-1);
    }
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);


#pragma warning(suppress : 6387)
    // Поиск конкретной функции библиотеки, которая заменит строки
    LPVOID replaceFuncPointer = (LPVOID)GetProcAddress(LoadLibraryA(DLL_PATH), "transferDllInjectParams");
    if (replaceFuncPointer == NULL) {
        std::cout << "Адрес заменяющей функции не найден" << std::endl;
        exit(-1);
    }
    // Размер параметров
    int paramSize = strlen(ORIGINAL_STRING) + 1;
    // Выделения памяти под параметры(по размеру строки)
    char* buffer = (char*)calloc(paramSize, sizeof(char));
    if (buffer == NULL) {
        std::cout << "Память под параметр не выделилась" << std::endl;
        exit(-1);
    }
    // Запись параметра для замены в память(сначала в буфер)
    memcpy(buffer, ORIGINAL_STRING, strlen(ORIGINAL_STRING) + 1);
    if (WriteProcessMemory(process, allocForStrings, buffer, paramSize, NULL) == 0) {
        std::cout << "Параметр не записался в память" << std::endl;
        exit(-1);
    }

    struct Params params = {
    pid,
    ORIGINAL_STRING,
    RESULT_STR
    };
    // Выделение памяти под структуру параметров и их запись, так как dll нужно отдельно выделить память под параметры
    LPVOID allocPwnParamPtr = VirtualAllocEx(process, NULL, sizeof(params), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (allocPwnParamPtr == NULL) {
        std::cout << "Память под структуру параметров не выделилась" << std::endl;
        exit(-11);
    }
    if (!WriteProcessMemory(process, allocPwnParamPtr, &params, sizeof(params), NULL)) {
        std::cout << "Структура параметров не записалась в память" << std::endl;
        exit(-1);
    }

    // Создание нового потока и указание его первым дейсвтием функцию замены строк в памяти
    thread = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)replaceFuncPointer, allocPwnParamPtr, 0, NULL);
    if (thread == NULL) {
        printf("Поток под заменяющую функцию не выделился");
        exit(-11);
    }
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
    CloseHandle(process);

    free(buffer);
}