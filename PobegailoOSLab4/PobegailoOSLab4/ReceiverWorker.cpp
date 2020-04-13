#include "ReceiverWorker.h"
#include "SyscallException.h"
#include "Message.h"

const std::string ReceiverWorker::mutexName = "BinaryFileMutex";

HANDLE ReceiverWorker::file = INVALID_HANDLE_VALUE;
HANDLE ReceiverWorker::fileMutex = nullptr;
std::string ReceiverWorker::fileName;
STARTUPINFO *ReceiverWorker::processStartupInfo = nullptr;
PROCESS_INFORMATION *ReceiverWorker::processes = nullptr;
HANDLE *ReceiverWorker::commEvents = nullptr;
DWORD ReceiverWorker::fileSize;
int ReceiverWorker::numberOfSenders;
DWORD ReceiverWorker::queueHead;
DWORD ReceiverWorker::queueTail;

void ReceiverWorker::writeZeroMessage()
{
    char arr[sizeof(Message)] = { 0 };
    if (!WriteFile
    (
        file,
        arr,
        sizeof(Message),
        NULL,
        NULL
    ))
    {
        throw SyscallException("Can't fill the message with zeros!", GetLastError());
    }
}

void ReceiverWorker::fillFileWithZeros
    (int numberOfRecords)
{
    for (int i = 0; i < numberOfRecords; ++i)
        writeZeroMessage();

    fileSize = numberOfRecords * sizeof(Message);
    queueHead = queueTail = 0;
    restoreFilePointer(&queueHead);
}

void ReceiverWorker::CreateEmptyBinaryFile
    (const std::string &fileName,
     int numberOfRecords)
{
    ReceiverWorker::fileName = fileName;

    SECURITY_ATTRIBUTES attributes;
    attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
    attributes.lpSecurityDescriptor = NULL;
    attributes.bInheritHandle = TRUE;

    file = CreateFile
    (
        fileName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        &attributes,
        CREATE_ALWAYS,
        FILE_FLAG_RANDOM_ACCESS,
        nullptr
    );
    if (file == INVALID_HANDLE_VALUE)
        throw SyscallException("Can't create the binary file!", GetLastError());

    fileMutex = CreateMutex
    (
        nullptr,
        FALSE,
        mutexName.c_str()
    );
    if (fileMutex == nullptr)
    {
        throw SyscallException("Can't create the mutex!", GetLastError());
    }
    else if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        throw SyscallException("The mutex already exists!", ERROR_ALREADY_EXISTS);
    }

    fillFileWithZeros(numberOfRecords);
}

void ReceiverWorker::saveFilePointer(LPDWORD filePointer)
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

void ReceiverWorker::restoreFilePointer(LPDWORD filePointer)
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

void ReceiverWorker::RunAndWaitForSenders
    (int numberOfSenders)
{
    ReceiverWorker::numberOfSenders = numberOfSenders;
    processes = new PROCESS_INFORMATION[numberOfSenders];
    processStartupInfo = new STARTUPINFO[numberOfSenders];
    commEvents = new HANDLE[numberOfSenders]();

    for (int i = 0; i < numberOfSenders; ++i)
    {
        ZeroMemory(processes + i, sizeof(PROCESS_INFORMATION));
        ZeroMemory(processStartupInfo + i, sizeof(STARTUPINFO));
        processStartupInfo[i].cb = sizeof(STARTUPINFO);
    }

    char eventName[32];
    for (int i = 0; i < numberOfSenders; ++i)
    {
        if (!CreateProcess
        (
            "../Debug/Sender.exe",
            (char*)(fileName + " " + std::to_string((DWORD)file)).c_str(),
            nullptr,
            nullptr,
            TRUE,
            CREATE_NEW_CONSOLE | CREATE_SUSPENDED,
            nullptr,
            nullptr,
            processStartupInfo + i,
            processes + i
        ))
        {
            throw SyscallException("Can't create a Sender process!", GetLastError());
        }

        sprintf_s(eventName, "Event#%u", processes[i].dwProcessId);
        commEvents[i] =
            CreateEvent
            (
                NULL,
                FALSE,
                FALSE,
                eventName
            );
        if (commEvents[i] == nullptr)
        {
            throw SyscallException("Can't create an event!", GetLastError());
        }

        ResumeThread(processes[i].hThread);
    }

    WaitForMultipleObjects(numberOfSenders, commEvents, TRUE, INFINITE);
}

std::string ReceiverWorker::ReadMessage()
{
    WaitForSingleObject(fileMutex, INFINITE);

    saveFilePointer(&queueTail);

    restoreFilePointer(&queueHead);
    DWORD tempHead = queueHead;
    Message receivedMsg;
    std::string result;

    if (!ReadFile
        (
            file,
            &receivedMsg,
            sizeof(Message),
            nullptr,
            nullptr
        ))
    {
        throw SyscallException("Can't read the message!", GetLastError());
    }

    result = receivedMsg.processID == 0
             ? "No message for the Receiver!"
             : "Message sent by Sender #" + std::to_string(receivedMsg.processID) +
               ": " + receivedMsg.message;
    restoreFilePointer(&tempHead);
    if (receivedMsg.processID != 0)
        writeZeroMessage();

    saveFilePointer(&queueHead);
    if (queueHead == fileSize)
        queueHead = 0;
    restoreFilePointer(&queueTail);

    ReleaseMutex(fileMutex);

    return result;
}

void ReceiverWorker::DisposeResources()
{
    if (file != INVALID_HANDLE_VALUE)
        CloseHandle(file);
    if (fileMutex != nullptr)
    {
        ReleaseMutex(fileMutex);
        CloseHandle(fileMutex);
    }
    if (processStartupInfo != nullptr)
        delete[] processStartupInfo;
    if (processes != nullptr)
    {
        for (int i = 0; i < numberOfSenders; ++i)
            if (processes[i].hProcess != nullptr)
            {
                CloseHandle(processes[i].hProcess);
                CloseHandle(processes[i].hThread);
            }
        delete[] processes;
    }
    if (commEvents != nullptr)
    {
        for (int i = 0; i < numberOfSenders; ++i)
            if (commEvents[i] != nullptr)
                CloseHandle(commEvents[i]);
        delete[] commEvents;
    }
}