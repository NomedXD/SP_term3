#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
#include <fstream>
#include <thread>
#include <regex>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <string.h>
#include "ThreadPool.h"
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <string>

#define BUF_SIZE 4096
// Исходные данные всего файла
std::vector<std::string> data;
// Вектор, где каждый элемент это массив строк для каждого отдельного потока
std::vector<std::vector<std::string>> sep_data;
LPCSTR pBuf;

std::vector<std::string> ReadFile()
{
	// Имя файла с данными
	std::string filePath = "E:/SP_term3/Lab4/text/file.txt";
	OFSTRUCT fileInfo{};
	// Открытие файла и получение дискриптора
	HFILE file = OpenFile(filePath.c_str(), &fileInfo, OF_READWRITE); 
	// Размер файла в байтах
	DWORD fileSize = GetFileSize((HANDLE)file, NULL);
	// Создание объекта сопоставления файла
	HANDLE hFileObject = CreateFileMapping((HANDLE)file, NULL, PAGE_READWRITE, 0, fileSize, NULL); 
	if (hFileObject == NULL) throw "Can't get descriptor of projected file object";
	// Создание отображения файла на пространство этого процесса
	pBuf = (LPCSTR)MapViewOfFile(hFileObject, FILE_MAP_ALL_ACCESS, 0, 0, fileSize); 
	if (pBuf == NULL) throw "Can't map file object to file";
	// Выделение памяти под данные из файла
	char* fileData = (char*)calloc(fileSize + 1, sizeof(char));
	if (fileData == NULL) throw "Can't allocate memory for file data";
	// Само чтение данных из файла(+ нуль терминатор)
	strncpy_s(fileData, fileSize + 1, pBuf, fileSize);
	// Нарезание всех данных на строки путем нахождения следующего токена \r\n
	std::vector<std::string> fileLines{};
	char* line = strtok(fileData, "\r\n");
	while (line != NULL)
	{
		fileLines.push_back(std::string(line));
		line = strtok(NULL, "\r\n");
	}
	// Закрытие дискрипторов
	CloseHandle((HANDLE)file);
	CloseHandle(hFileObject);

	return fileLines;
}

short ReadThreadsNum()
{
	std::cout << "Input number of threads: ";
	std::string threadsNumber;
	std::cin >> threadsNumber;
	std::cout << "\n";
	std::regex numberRegex("^([0-9]+)$");
	// Првоерка на корректный ввоод
	if (!std::regex_match(threadsNumber, numberRegex) || stoi(threadsNumber) > data.size() || stoi(threadsNumber) <= 0) throw "Incorrect data";
	return stoi(threadsNumber);
}

// Сортировка строк, которые отдельно выделены для каждого потока
// Принимает указатель на данные любого типа. К нужному типу приводится уже в самой функции
void SortSeparateParts(void* fragment)
{
	std::vector<std::string>* lines = (std::vector<std::string>*)fragment;
	std::sort(lines->begin(), lines->end());
}

template<typename T>
class Sort
{
public:
	static void MergeSort(std::vector<T>& left, std::vector<T>& right, std::vector<T>& bars)
	{
		int nL = left.size();
		int nR = right.size();
		int i = 0, j = 0, k = 0;

		while (j < nL && k < nR)
		{
			if (left[j] < right[k]) {
				bars[i] = left[j];
				j++;
			}
			else {
				bars[i] = right[k];
				k++;
			}
			i++;
		}
		while (j < nL) {
			bars[i] = left[j];
			j++; i++;
		}
		while (k < nR) {
			bars[i] = right[k];
			k++; i++;
		}
	}

	static void sort(std::vector<T>& bar)
	{
		if (bar.size() <= 1) return;

		int mid = bar.size() / 2;
		std::vector<T> left;
		std::vector<T> right;

		for (size_t j = 0; j < mid; j++)
			left.push_back(bar[j]);
		for (size_t j = 0; j < (bar.size()) - mid; j++)
			right.push_back(bar[mid + j]);

		sort(left);
		sort(right);
		MergeSort(left, right, bar);
	}
};

// Создание задания для отдельного потока
void CreateTasks(std::vector<std::string> indata, short quantity, Queue* queue)
{
	// Смотрим, четное ли количество будущих строк для каждого отдельного потока
	bool isEven = indata.size() % quantity == 0;
	// Само количество строк для каждого отдельного потока
	int standardStringCapacity = indata.size() / quantity;
	// Для последнего потока корректируем, так как ему достанется меньше строк
	if (!isEven)
		standardStringCapacity = indata.size() / quantity + 1;
	// Глобальный счетчик
	int globalCounter = 0;
	// Проход по всем строкам исходных данных
	for (int i = 0; i < quantity; i++)
	{
		// Очистка на всякий случай
		sep_data[i].clear();
		int tmpCounter = 0;
		// Пока мы не достигним количества строк для каждого отдельного потока
		while (standardStringCapacity != tmpCounter)
		{
			if (globalCounter == indata.size())
				break;
			std::string str = indata[globalCounter];
			// Добавление строк в элемент separatedData(строки каждого отдельного потока)
			if (str != "") {
				sep_data[i].push_back(str);
			}
			// Уменьшение глобальных строк на единицу
			globalCounter++;
			// Уменьшение количества оставшихся строк для добавления потоку
			tmpCounter++;
		}
		// Создание задачи. Первое дейсвтие задачи - сортировка его строк, данные - его собственные строки для сортировки
		Task* newtask = new Task(&SortSeparateParts, (void*)&sep_data[i]);
		// Непосредственно добавление задачи в общую очередь, из которой потом они будут выбираться
		queue->add(newtask);
	}
}

int main()
{
	try
	{
		// Чтение данных из файла
		data = ReadFile();
		// Чтение количества потоков
		short threadsCount = ReadThreadsNum();
		// Создание очереди задач и самих задач по количеству потоков
		sep_data.resize(threadsCount);
		Queue* queue = new Queue();
		CreateTasks(data, threadsCount, queue);
		data.clear();
		// Создание пула потоков и передача ссылки на очередь задач, из которой конкретный поток извлечет задаччу
		ThreadPool* threads = new ThreadPool(threadsCount, queue);
		// Основной поток ждет выполнения всех остальных(пока они посортируют)
		threads->WaitAll();
		delete(threads);
		delete(queue);
		// Формирование итогового массива с отсортированными частями
		for (const auto& fragment : sep_data)
		{
			data.insert(data.begin(), fragment.begin(), fragment.end());
		}
		// Слияние всех строк
		Sort<std::string>::sort(data);
		// Формирование строк(добавление переноса) для записи в файл
		std::string joinedStr = "";
		for (const auto& str : data)
		{
			joinedStr += str + "\r\n";
		}
		joinedStr.pop_back();
		joinedStr.pop_back();
		// Копирование данных их получившегося массива в отображение файла
		CopyMemory((PVOID)pBuf, joinedStr.c_str(), strlen(joinedStr.c_str()) * sizeof(char));
		UnmapViewOfFile(pBuf);
		return 0;
	}
	catch (const char* message)
	{
		std::cout << message << std::endl;
	}
}
