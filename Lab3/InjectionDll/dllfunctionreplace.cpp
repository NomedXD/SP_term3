#include <Windows.h>
#include <cstdlib>
#include "dllfunctionreplace.h"
#include <iostream>

// ��������� ��� ��������
struct Params {
	DWORD pid;
	// �������� ������
	char originalStr[10];
	// �������� ������
	char resultStr[10];
};


void transferDllInjectParams(struct Params* params) {
	std::cout << "����� ��������� ������� � ������ ����� � ������" << std::endl;
	// ����� ����� ������� ������ ������ � ������
	ReplaceStringInMemory(params->pid, params->originalStr, params->resultStr);
}
// ������� �������� � ������ �������� ���� ������ �� ������
void ReplaceStringInMemory(DWORD PID, const char* originalStr, const char* destString)
{
	// ��������� ����������� �������� �� ��� PID
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	// ���� �� �� NULL
	if (hProcess)
	{
		// ������ �������� ������
		size_t srcStrLen = strlen(originalStr);
		// �������� �������� � ������� ������� ����������
		SYSTEM_INFO systemInfo;
		// �������� ���������� � ��������� ������� � ����������� �������� ������������ ��������. 
		// ������� VirtualQuery � VirtualQueryEx ���������� ��� ���������
		MEMORY_BASIC_INFORMATION memoryInfo;
		// ���������� ��������� ��������� �����������
		GetSystemInfo(&systemInfo);
		// ��������� �� ������ ����������� ������ ��������
		char* pointer = 0;
		// ��������� ��� ������(���� �� ������)
		SIZE_T lpRead = 0;
		// ������ �� ���� ������, ��������� ��������
		// lpMaximumApplicationAddress - ��������� �� ����� ������� ����� ������, ��������� ����������� � ����������� DLL
		while (pointer < systemInfo.lpMaximumApplicationAddress)
		{
			// ��������� �������� � ��������� ������� � ����������� �������� ������������ ���������� ��������
			// pointer - ������� ����� ������� ������������� �������, ����������� ���������� � ����� memoryInfo
			int sz = VirtualQueryEx(hProcess, pointer, &memoryInfo, sizeof(MEMORY_BASIC_INFORMATION));
			if (sz == sizeof(MEMORY_BASIC_INFORMATION))
			{
				// ������������� �� �������� ���������� ����� � ������ � ����� �� ������ � ������
				// ��� �������, ������� ��������, ������ ���� ��������, ��� �������� ����������� �������.
				if ((memoryInfo.State == MEM_COMMIT) && memoryInfo.AllocationProtect == PAGE_READWRITE)
				{
					// ����� ��������� �� ������ �������
					pointer = (char*)memoryInfo.BaseAddress;
					try {
						// ��������� ������ ��� ������ ������ �� ������
						BYTE* readedData = (BYTE*)malloc(memoryInfo.RegionSize);
						// ������ �� ������ ������
						if (ReadProcessMemory(hProcess, pointer, readedData, memoryInfo.RegionSize, &lpRead))
						{
							// ��������� ��������� �������� � ��������� ������� ������
							for (size_t i = 0; i < (lpRead - srcStrLen); ++i)
							{
								// ��������������� ��������� ����������� ������ � ���� �������
								if (memcmp(originalStr, &readedData[i], srcStrLen) == 0)
								{
									// ����������� ��������� ��� ����������� ����������
									char* replaceMemory = pointer + i;
									// ������ ��������
									for (int j = 0; (j < (strlen(destString)) && j < (strlen(originalStr))); j++)
									{
										replaceMemory[j] = destString[j];
									}
									// ���� ����������
									replaceMemory[strlen(destString)] = 0;
								}
							}
						}
						// ������������ ������ ����������� ������ ������
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