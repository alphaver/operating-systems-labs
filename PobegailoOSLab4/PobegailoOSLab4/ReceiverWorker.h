#ifndef POBEGOS_LAB4_RECVWORKER_H

#define POBEGOS_LAB4_RECVWORKER_H

#include <string>
#include <vector>
#include <exception>
#include <windows.h>

class ReceiverWorker
{
private:
    static const std::string mutexName;

	static HANDLE file;
	static HANDLE fileMutex;
    static std::string fileName;
	static PROCESS_INFORMATION *processes;
	static STARTUPINFO *processStartupInfo;
    static HANDLE *commEvents;
	static DWORD fileSize;
    static int numberOfSenders;
    static DWORD queueHead;
    static DWORD queueTail;

    static void writeZeroMessage();
	static void fillFileWithZeros(int);
    static void saveFilePointer(LPDWORD);
    static void restoreFilePointer(LPDWORD);

public:
	static void CreateEmptyBinaryFile(const std::string &, int);
	static void RunAndWaitForSenders(int);
	static std::string ReadMessage();
	static void DisposeResources();
};

#endif // POBEGOS_LAB4_RECVWORKER_H