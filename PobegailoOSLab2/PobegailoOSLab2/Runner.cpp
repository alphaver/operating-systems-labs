#include "Runner.h"
#include <iostream>
#include <algorithm>
#include <iterator>

double Runner::minimum;
double Runner::maximum;
double Runner::average;
CRITICAL_SECTION Runner::consoleSection;
bool Runner::runCalled = false;
std::vector<double> Runner::array;

DWORD __stdcall Runner::minMaxThreadProc(LPVOID)
{
	if (array.size() == 0)
		return 0xCAFEBABE;

	Runner::minimum = Runner::maximum = Runner::array[0];
	for (std::size_t i = 0; i < Runner::array.size(); ++i)
	{
		Runner::minimum = min(Runner::minimum, Runner::array[i]);
		Sleep(7);
		Runner::maximum = max(Runner::maximum, Runner::array[i]);
		Sleep(7);
	}

	EnterCriticalSection(&Runner::consoleSection);
	std::cout << "Minimum: " << Runner::minimum << ", "
			  << "maximum: " << Runner::maximum << "\n";
	LeaveCriticalSection(&Runner::consoleSection);

	return 0;
}

DWORD __stdcall Runner::averageThreadProc(LPVOID)
{
	if (array.size() == 0)
		return 0xCAFEBABE;

	Runner::average = 0;
	for (std::size_t i = 0; i < Runner::array.size(); ++i)
	{
		Runner::average += Runner::array[i];
		Sleep(12);
	}
	Runner::average /= Runner::array.size();

	EnterCriticalSection(&Runner::consoleSection);
	std::cout << "Average: " << Runner::average << "\n";
	LeaveCriticalSection(&Runner::consoleSection);

	return 0;
}

HANDLE Runner::runThread
	(LPTHREAD_START_ROUTINE threadProc,
	 const std::string &name)
{
	HANDLE result = CreateThread
		(
			nullptr,
			0,
			threadProc,
			nullptr,
			0,
			nullptr
		);
	if (result == nullptr)
		throw new RunnerException("Problem with thread " + name + "!", -4);

	return result;
}

void Runner::ReadArray()
{
	std::cout << "Input the size of the array: ";
	int size;
	std::cin >> size;
	Runner::array.resize(size);

	std::cout << "Input the elements of the array: ";
	int element;
	for (int i = 0; i < size; ++i)
	{
		std::cin >> element;
		Runner::array[i] = element;
	}

	Runner::runCalled = false;
}

void Runner::RunThreads()
{
	InitializeCriticalSection(&Runner::consoleSection); 

	HANDLE minMaxHandle = runThread(minMaxThreadProc, "MinMax");
	HANDLE avgHandle = runThread(averageThreadProc, "Average");

	WaitForSingleObject(minMaxHandle, INFINITE);
	WaitForSingleObject(avgHandle, INFINITE);

	DWORD exitCode;
	GetExitCodeThread(minMaxHandle, &exitCode);
	if (exitCode != 0)
		throw new RunnerException("Thread MinMax has exited in an unusual way, " \
								  "exit code = ", exitCode);
	GetExitCodeThread(avgHandle, &exitCode);
	if (exitCode != 0)
		throw new RunnerException("Thread Average has exited in an unusual way, " \
								  "exit code = ", exitCode);

	CloseHandle(minMaxHandle);
	CloseHandle(avgHandle);
	DeleteCriticalSection(&Runner::consoleSection);

	Runner::runCalled = true;
}

void Runner::SubstituteMinMax()
{
	if (!Runner::runCalled)
		throw new RunnerException("Can't substitute without computing!", -5);
	if (Runner::array.size() == 0)
		throw new RunnerException("Can't operate on an empty array!", -6);

	for (int i = 0; i < Runner::array.size(); ++i)
		if (Runner::array[i] == Runner::minimum ||
			Runner::array[i] == Runner::maximum)
		{
			Runner::array[i] = Runner::average;
		}
}

void Runner::PrintArray()
{
	std::copy(Runner::array.begin(), Runner::array.end(),
			  std::ostream_iterator<double>(std::cout, " "));
	std::cout << '\n';
}