#include <Windows.h>
#include <cstdlib>
#include "dllfunctionreplace.h"
#include <iostream>

// Структура для удобства
struct Params {
	DWORD pid;
	// Исходная строка
	char originalStr[10];
	// Итоговая строка
	char resultStr[10];
};


void transferDllInjectParams(struct Params* params) {
	std::cout << "Здесь произошел инджект и замена строк в памяти" << std::endl;
	// Вызов самой функции замены строки в памяти
	ReplaceStringInMemory(params->pid, params->originalStr, params->resultStr);
}
// Функция заменяет в памяти процесса одну строку на другую
void ReplaceStringInMemory(DWORD PID, const char* originalStr, const char* destString)
{
	// Получение дискриптора процесса по его PID
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	// Если он не NULL
	if (hProcess)
	{
		// Размер исходной строки
		size_t srcStrLen = strlen(originalStr);
		// Содержит сведения о текущей системе компьютера
		SYSTEM_INFO systemInfo;
		// Содержит информацию о диапазоне страниц в виртуальном адресном пространстве процесса. 
		// Функции VirtualQuery и VirtualQueryEx используют эту структуру
		MEMORY_BASIC_INFORMATION memoryInfo;
		// Заполнение структуры системной информацией
		GetSystemInfo(&systemInfo);
		// Указатель на начало виртуальной памяти процесса
		char* pointer = 0;
		// Указатель для чтения(тоже на начало)
		SIZE_T lpRead = 0;
		// Проход по всей памяти, доступной процессу
		// lpMaximumApplicationAddress - указатель на самый высокий адрес памяти, доступный приложениям и библиотекам DLL
		while (pointer < systemInfo.lpMaximumApplicationAddress)
		{
			// Извлекает сведения о диапазоне страниц в виртуальном адресном пространстве указанного процесса
			// pointer - базовый адрес области запрашиваемых страниц, считываение происходит в буфер memoryInfo
			int sz = VirtualQueryEx(hProcess, pointer, &memoryInfo, sizeof(MEMORY_BASIC_INFORMATION));
			if (sz == sizeof(MEMORY_BASIC_INFORMATION))
			{
				// Действительно ли выделено физическое место в памяти и можно ли читать и писать
				// Вся область, которая читается, должна быть доступна, или операция завершается ошибкой.
				if ((memoryInfo.State == MEM_COMMIT) && memoryInfo.AllocationProtect == PAGE_READWRITE)
				{
					// Сброс указателя на начало страниц
					pointer = (char*)memoryInfo.BaseAddress;
					try {
						// Выделение памяти под чтение данных из памяти
						BYTE* readedData = (BYTE*)malloc(memoryInfo.RegionSize);
						// Чтение из памяти данных
						if (ReadProcessMemory(hProcess, pointer, readedData, memoryInfo.RegionSize, &lpRead))
						{
							// Сравнение считанных символов с символами искомой строки
							for (size_t i = 0; i < (lpRead - srcStrLen); ++i)
							{
								// Непосредственно сравнение прочитанных данных в двух буферах
								if (memcmp(originalStr, &readedData[i], srcStrLen) == 0)
								{
									// Передвигаем указатель для дальнейшего считывания
									char* replaceMemory = pointer + i;
									// ЗАмена символов
									for (int j = 0; (j < (strlen(destString)) && j < (strlen(originalStr))); j++)
									{
										replaceMemory[j] = destString[j];
									}
									// Нуль терминатор
									replaceMemory[strlen(destString)] = 0;
								}
							}
						}
						// Освобождение буфера прочитанной порции данных
						free(readedData);
					}
					catch (std::bad_alloc& e) { printf("%s\n", e.what()); }

				}
			}
			pointer += memoryInfo.RegionSize;
		}

		CloseHandle(hProcess);
	}
}