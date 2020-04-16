#include "SenderWorker.h"
#include "../PobegailoOSLab4/SyscallException.h"
#include "../PobegailoOSLab4/Message.h"

const std::string SenderWorker::mutexName = "BinaryFileMutex";

HANDLE SenderWorker::file = INVALID_HANDLE_VALUE;
HANDLE SenderWorker::fileMutex = nullptr;
HANDLE SenderWorker::commEvent = nullptr;
std::string SenderWorker::fileName;
DWORD SenderWorker::fileSize;
DWORD SenderWorker::processID;

void SenderWorker::saveFilePointer(LPDWORD filePointer)
{
    *filePointer =
        SetFilePointer
        (
            file,
            0,
            nullptr,
            FILE_CURRENT
        );
    if (*filePointer == INVALID_SET_FILE_POINTER)
    {
        throw SyscallException("Can't save the file pointer!", GetLastError());
    }
}

void SenderWorker::restoreFilePointer(LPDWORD filePointer)
{
    *filePointer =
        SetFilePointer
        (
            file,
            *filePointer,
            nullptr,
            FILE_BEGIN
        );
    if (*filePointer == INVALID_SET_FILE_POINTER)
    {
        throw SyscallException("Can't restore the file pointer!", GetLastError());
    }
}

void SenderWorker::InitializeSender()
{
    processID = GetCurrentProcessId();

    fileMutex =
        OpenMutex
        (
            MUTEX_ALL_ACCESS,
            FALSE,
            mutexName.c_str()
        );
    if (fileMutex == nullptr)
    {
        throw SyscallException("Can't open mutex!", GetLastError());
    }

    char eventName[32];
    sprintf_s(eventName, "Event#%u", processID);
    commEvent =
        OpenEvent
        (
            EVENT_MODIFY_STATE,
            FALSE,
            eventName
        );
    if (commEvent == nullptr)
    {
        throw SyscallException("Can't open event!", GetLastError());
    }
}

unsigned long SenderWorker::GetProcessID()
{
    return GetCurrentProcessId();
}

void SenderWorker::OpenBinaryFile
    (const std::string &fileName,
     const std::string &handle)
{
    SenderWorker::fileName = fileName;
    file = (HANDLE)strtoul(handle.c_str(), nullptr, 10);

    fileSize = GetFileSize(file, nullptr);
    if (fileSize == INVALID_FILE_SIZE)
    {
        throw SyscallException("Can't determine the file size!", GetLastError());
    }
}

void SenderWorker::SignalReceiver()
{
    SetEvent(commEvent);
}

void SenderWorker::SendMessageToReceiver
    (const std::string &message)
{
    Message currentMsg;
    currentMsg.processID = processID;
    DWORD filePosition;

    while (currentMsg.processID != 0)
    {
        WaitForSingleObject(fileMutex, INFINITE);
        
        saveFilePointer(&filePosition);
        if (!ReadFile
            (
                file,
                &currentMsg,
                sizeof(Message),
                nullptr,
                nullptr
            ))
        {
            throw SyscallException("Can't read from the file!", GetLastError());
        }

        restoreFilePointer(&filePosition);
        if (currentMsg.processID != 0)
        {
            ReleaseMutex(fileMutex);
            Sleep(5000);
        }
    }

    currentMsg.processID = processID;
    strncpy_s(currentMsg.message, message.c_str(), 20);
    currentMsg.message[19] = '\0';

    if (!WriteFile
        (
            file,
            &currentMsg,
            sizeof(Message),
            nullptr,
            nullptr
        ))
    {
        throw SyscallException("Can't write to the file!", GetLastError());
    }

    saveFilePointer(&filePosition);
    if (filePosition == fileSize)
    {
        filePosition = 0;
        restoreFilePointer(&filePosition);
    }

    ReleaseMutex(fileMutex);
}

void SenderWorker::DisposeResources()
{
    if (file != INVALID_HANDLE_VALUE)
        CloseHandle(file);
    if (fileMutex != nullptr)
    {
        ReleaseMutex(fileMutex);
        CloseHandle(fileMutex);
    }
    if (commEvent != nullptr)
        CloseHandle(commEvent);
}