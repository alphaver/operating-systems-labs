#ifndef POBEGOS_LAB4_SENDERWORKER_H

#define POBEGOS_LAB4_SENDERWORKER_H

#include <windows.h>
#include <string>

class SenderWorker
{
private:
    static const std::string mutexName;

    static HANDLE file;
    static HANDLE fileMutex;
    static HANDLE commEvent;
    static std::string fileName;
    static DWORD fileSize;
    static DWORD processID;

    static void saveFilePointer(LPDWORD);
    static void restoreFilePointer(LPDWORD);

public:
    static void InitializeSender();
    static void OpenBinaryFile(const std::string &, const std::string &);
    static void SignalReceiver();
    static void SendMessageToReceiver(const std::string &);
    static void DisposeResources();
};

#endif // POBEGOS_LAB4_SENDERWORKER_H