#include "ServerNamedPipe.h"
#include "InputOutputException.h"

const std::string ServerNamedPipe::PIPE_PREFIX = R"(\\.\pipe\)";

ServerNamedPipe::ServerNamedPipe(const std::string &pipeName, std::uint32_t timeout)
    : pipeName(pipeName), timeout(timeout)
{
    ZeroMemory(&overlapped, sizeof(OVERLAPPED));
    hNamedPipe = CreateNamedPipe
                 ((PIPE_PREFIX + pipeName).c_str(),
                  PIPE_ACCESS_DUPLEX,
                  PIPE_TYPE_MESSAGE | 
                  PIPE_READMODE_MESSAGE |
                  PIPE_WAIT,
                  1,
                  sizeof(PipeMessage),
                  sizeof(PipeMessage),
                  timeout,
                  nullptr);
    if (hNamedPipe == INVALID_HANDLE_VALUE)
        throw InputOutputException("Can't create the pipe " + pipeName + "!");
    overlapped.hEvent = CreateEvent
                        (nullptr,
                         TRUE,
                         FALSE,
                         nullptr);
    if (overlapped.hEvent == nullptr)
        throw InputOutputException("Can't create the completion event for the pipe " + pipeName + "!");
}

ServerNamedPipe::~ServerNamedPipe()
{
    if (hNamedPipe != nullptr && hNamedPipe != INVALID_HANDLE_VALUE)
    {
        DisconnectNamedPipe(hNamedPipe);
        CloseHandle(hNamedPipe);
    }
}

const std::string & ServerNamedPipe::GetPipeName() const
{
    return pipeName;
}

bool ServerNamedPipe::TryConnectToClient()
{
    BOOL result = ConnectNamedPipe(hNamedPipe, &overlapped);

    if (!result)
        switch (GetLastError())
        {
        case ERROR_PIPE_CONNECTED:
            result = TRUE;
            break;

        case ERROR_IO_PENDING:
            if (WaitForSingleObject(overlapped.hEvent, timeout) == WAIT_OBJECT_0)
                result = GetOverlappedResult(hNamedPipe, &overlapped, nullptr, FALSE);
            else
            {
                CancelIo(hNamedPipe);
                result = FALSE;
            }
            break;

        default:
            result = FALSE;
        }

    return result;
}

void ServerNamedPipe::SendPipeMessage(const PipeMessage &pipeMessage)
{
    DWORD sentSize;
    BOOL result = WriteFile(hNamedPipe, &pipeMessage, sizeof(pipeMessage), &sentSize, &overlapped);
    if (!result && GetLastError() == ERROR_IO_PENDING)
        result = GetOverlappedResult(hNamedPipe, &overlapped, &sentSize, TRUE);
    else if (!result && GetLastError() == ERROR_BROKEN_PIPE)
        throw new InputOutputException("Can't send a message via the pipe " + pipeName + " because it's broken!");
    
    if (!result || sentSize != sizeof(pipeMessage))
        throw InputOutputException("Unknown error while sending the message via the pipe " + pipeName + "!");
}

void ServerNamedPipe::ReceivePipeMessage(PipeMessage &pipeMessage)
{
    DWORD sentSize;
    BOOL result = ReadFile(hNamedPipe, &pipeMessage, sizeof(pipeMessage), &sentSize, &overlapped);
    if (!result && GetLastError() == ERROR_IO_PENDING)
        result = GetOverlappedResult(hNamedPipe, &overlapped, &sentSize, TRUE);
    else if (!result && GetLastError() == ERROR_BROKEN_PIPE)
        throw new InputOutputException("Can't receive a message via the pipe " + pipeName + " because it's broken!");

    if (!result || sentSize != sizeof(pipeMessage))
        throw InputOutputException("Unlnown error while receiving the message via the pipe " + pipeName + "!");
}

bool ServerNamedPipe::DisconnectFromClient()
{
    return DisconnectNamedPipe(hNamedPipe);
}
