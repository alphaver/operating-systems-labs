#include "Runner.h"
#include <iostream>
#include <algorithm>
#include <iterator>
#include <string>
#include <cstdlib>

int *Runner::array;
int Runner::arraySize;
HANDLE *Runner::markers = nullptr;
int Runner::numOfThreads = 0;
HANDLE *Runner::stopEvents = nullptr;
HANDLE *Runner::finishEvents = nullptr;
HANDLE Runner::startEvents[2] = { nullptr, nullptr };
int Runner::currWaitEvent = 0;
CRITICAL_SECTION Runner::console;
CRITICAL_SECTION Runner::arraySection;

DWORD __stdcall Runner::markerThreadProc(LPVOID number)
{
	WaitForSingleObject(startEvents[currWaitEvent], INFINITE);

	int threadNumber = (int)number;
	std::srand(threadNumber);
	int markedNumber = 0;

	while (true)
	{
		int idx = std::rand() % arraySize;
		EnterCriticalSection(&arraySection);
		if (array[idx] == 0)
		{
			Sleep(5);
			array[idx] = threadNumber + 1;
			Sleep(5);
            LeaveCriticalSection(&arraySection);

			++markedNumber;
		}
		else
		{
			LeaveCriticalSection(&arraySection);
			
			EnterCriticalSection(&console);
			std::cout << "I'm Marker #" << (threadNumber + 1)
					  << " and I can't work anymore after "
					  << markedNumber << " marked elements "
					  << "because I found a conflict at index " << (idx + 1) << "!\n";
			LeaveCriticalSection(&console);

			SetEvent(finishEvents[threadNumber]);

			HANDLE awaitableEvents[] = { stopEvents[threadNumber], startEvents[currWaitEvent ^ 1] };
			int retCode = WaitForMultipleObjects(2, awaitableEvents, FALSE, INFINITE);
			
			if (retCode == WAIT_OBJECT_0)
			{
				for (int i = 0; i < arraySize; ++i)
					if (array[i] == threadNumber + 1) 
                        array[i] = 0;

				return 0;
			}
		}
	}
}

void Runner::createEvents()
{
	if (numOfThreads == 0)
		return;

	startEvents[0] = CreateEvent
	(
		nullptr,
		TRUE,
		FALSE,
		nullptr
	);

	if (startEvents[0] == nullptr)
	{
		disposeEvents();
		throw new RunnerException("Can't create the start event!", -2);
	}

	startEvents[1] = CreateEvent
	(
		nullptr,
		TRUE,
		FALSE,
		nullptr
	);

	if (startEvents[1] == nullptr)
	{
		disposeEvents();
		throw new RunnerException("Can't create the start event!", -2);
	}

	stopEvents = new HANDLE[numOfThreads]();
	finishEvents = new HANDLE[numOfThreads]();
	for (int i = 0; i < numOfThreads; ++i)
	{
		stopEvents[i] = CreateEvent
		(
			nullptr,
			TRUE,
			FALSE,
			nullptr
		);
		
		if (stopEvents[i] == nullptr)
		{
			disposeEvents();
			throw new RunnerException("Can't create a stop event!", -3);
		}

		finishEvents[i] = CreateEvent
		(
			nullptr,
			TRUE,
			FALSE,
			nullptr
		);

		if (stopEvents[i] == nullptr)
		{
			disposeEvents();
			throw new RunnerException("Can't create a finish event!", -4);
		}
	}
}

void Runner::disposeEvents()
{
	for (int i = 0; i < numOfThreads; ++i)
	{
		if (stopEvents != nullptr && stopEvents[i] != nullptr)
			CloseHandle(stopEvents[i]);
		if (finishEvents != nullptr && finishEvents[i] != nullptr)
			CloseHandle(finishEvents[i]);
	}
	if (startEvents[0] != nullptr)
		CloseHandle(startEvents[0]);
	if (startEvents[1] != nullptr)
		CloseHandle(startEvents[1]);
	if (stopEvents != nullptr)
		delete[] stopEvents;
	if (finishEvents != nullptr)
		delete[] finishEvents;

	stopEvents = finishEvents = nullptr;
	startEvents[0] = startEvents[1] = nullptr;
}

HANDLE Runner::createMarker(int number)
{
	HANDLE result = CreateThread
	(
		nullptr,
		0,
		markerThreadProc,
		(LPVOID) number,
		0,
		nullptr
	);
	if (result == nullptr)
		throw new RunnerException("Problem with thread #" + std::to_string(number) + "!", -1);

	return result;
}

void Runner::initializeSyncEnvironment()
{
	if (numOfThreads == 0)
		return;
	
	if (markers != nullptr)
		disposeSyncEnvironment();

	createEvents();
	InitializeCriticalSection(&console);
	InitializeCriticalSection(&arraySection);
	markers = new HANDLE[numOfThreads]();	
}

void Runner::disposeSyncEnvironment()
{
	DeleteCriticalSection(&console);
	DeleteCriticalSection(&arraySection);

	for (int i = 0; i < numOfThreads; ++i)
	{
		if (markers != nullptr && markers[i] != nullptr)
			CloseHandle(markers[i]);
	}
	if (markers != nullptr)
		delete[] markers;

	disposeEvents();
	numOfThreads = 0;
	markers = nullptr;
}

void Runner::CreateArray(int arraySize)
{
	Runner::arraySize = arraySize;
	array = new int[arraySize]();
}

void Runner::PrintArray()
{
	std::copy(array, array + arraySize,
			  std::ostream_iterator<int>(std::cout, " "));
	std::cout << '\n';
}

void Runner::CreateAndRunMarkers(int numOfMarkers)
{
	numOfThreads = numOfMarkers;
	initializeSyncEnvironment();
	for (int i = 0; i < numOfMarkers; ++i)
		markers[i] = createMarker(i);
	SetEvent(startEvents[currWaitEvent]);
}

void Runner::WaitForMarkers()
{
	WaitForMultipleObjects(numOfThreads, finishEvents, TRUE, INFINITE);
}

bool Runner::TryKillMarker(int markerToKill)
{
	if (markers[markerToKill] == nullptr)
		return false;

	SetEvent(stopEvents[markerToKill]);
	WaitForSingleObject(markers[markerToKill], INFINITE);
	CloseHandle(markers[markerToKill]);
	markers[markerToKill] = nullptr;

	return true;
}

void Runner::ResumeMarkers()
{
	ResetEvent(startEvents[currWaitEvent]);
	for (int i = 0; i < numOfThreads; ++i)
		if (markers[i] != nullptr)
			ResetEvent(finishEvents[i]);
	currWaitEvent ^= 1;
	SetEvent(startEvents[currWaitEvent]);
}

void Runner::Dispose()
{
	disposeSyncEnvironment();
	delete[] array;
}