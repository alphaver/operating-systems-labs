#ifndef POBEGOS_LAB2_RUNNER_H

#define POBEGOS_LAB2_RUNNER_H

#include <windows.h>
#include <string>
#include <exception>
#include <vector>
#include <iostream>

typedef unsigned long ulong;

class Runner
{
private:
	static double minimum, maximum, average;
	static CRITICAL_SECTION consoleSection;
	static bool runCalled;
	static std::vector<double> array;

	static DWORD __stdcall minMaxThreadProc(LPVOID);
	static DWORD __stdcall averageThreadProc(LPVOID);

	static HANDLE runThread(LPTHREAD_START_ROUTINE, const std::string &);

public:
	static void ReadArray();
	static void RunThreads();
	static void SubstituteMinMax();
	static void PrintArray();
};

class RunnerException : public std::exception
{
private:
	ulong errCode;
public:
	RunnerException(const std::string &message, ulong errCode)
		: std::exception(message.c_str()), errCode(errCode) {}

	ulong GetErrCode()
	{
		return errCode;
	}
};

#endif