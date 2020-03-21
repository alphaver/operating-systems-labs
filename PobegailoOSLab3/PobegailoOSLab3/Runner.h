#ifndef POBEGOS_LAB3_RUNNER_H

#define POBEGOS_LAB3_RUNNER_H

#include <windows.h>
#include <string>
#include <exception>
#include <vector>
#include <iostream>

typedef unsigned long ulong;

class Runner
{
private:
	static int *array;
	static int arraySize;
	static HANDLE *markers;
	static int numOfThreads;
	static HANDLE *stopEvents;
	static HANDLE *finishEvents;
	static HANDLE startEvents[2];
	static int currWaitEvent;
	static CRITICAL_SECTION console;
	static CRITICAL_SECTION arraySection;

	static DWORD __stdcall markerThreadProc(LPVOID);

	static void createEvents();
	static void disposeEvents();
	static HANDLE createMarker(int);
	static void initializeSyncEnvironment();
	static void disposeSyncEnvironment();

public:
	static void CreateArray(int);
	static void PrintArray();
	static void CreateAndRunMarkers(int);
	static void WaitForMarkers();
	static bool TryKillMarker(int);
	static void ResumeMarkers();
	static void Dispose();
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