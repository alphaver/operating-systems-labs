#include "ClientNamedPipe.h"
#include "../Server/InputOutputException.h"

const std::string ClientNamedPipe::PIPE_PREFIX = R"(\\.\pipe\)";

ClientNamedPipe::ClientNamedPipe(const std::string &pipeName)
    : pipeName(pipeName), hNamedPipe(nullptr) {}

ClientNamedPipe::~ClientNamedPipe()
{
    if (hNamedPipe != nullptr)
        DisconnectFromServer();
}

const std::string & ClientNamedPipe::GetPipeName() const
{
    return pipeName;
}

bool ClientNamedPipe::TryConnectToServer()
{
    hNamedPipe = CreateFile
                 ((PIPE_PREFIX + pipeName).c_str(),
                 GENERIC_READ | GENERIC_WRITE,
                 0, 
                 nullptr,
                 OPEN_EXISTING,
                 0,
                 nullptr);
    if (hNamedPipe == INVALID_HANDLE_VALUE)
    {
        hNamedPipe = nullptr;
        return false;
    }

    DWORD dwMode = PIPE_READMODE_MESSAGE;
    BOOL success = SetNamedPipeHandleState
                   (hNamedPipe,
                    &dwMode,
                    nullptr,
                    nullptr);
    return success;
}

void ClientNamedPipe::SendPipeMessage(const PipeMessage &pipeMessage)
{
    DWORD sentSize;
    BOOL result = WriteFile(hNamedPipe, &pipeMessage, sizeof(pipeMessage), &sentSize, nullptr);
    if (!result && GetLastError() == ERROR_BROKEN_PIPE)
        throw new InputOutputException("Can't receive a message via the pipe " + pipeName + " because it's broken!");
    if (!result || sentSize != sizeof(pipeMessage))
        throw InputOutputException("Error while receiving the message by the pipe " + pipeName + "!");
}

void ClientNamedPipe::ReceivePipeMessage(PipeMessage & pipeMessage)
{
    DWORD sentSize;
    BOOL result = ReadFile(hNamedPipe, &pipeMessage, sizeof(pipeMessage), &sentSize, nullptr);
    if (!result && GetLastError() == ERROR_BROKEN_PIPE)
        throw new InputOutputException("Can't receive a message via the pipe " + pipeName + " because it's broken!");
    if (!result || sentSize != sizeof(pipeMessage))
        throw InputOutputException("Error while receiving the message by the pipe " + pipeName + "!");
}

bool ClientNamedPipe::DisconnectFromServer()
{
    BOOL result = CloseHandle(hNamedPipe);
    if (result)
        hNamedPipe = nullptr;
    return result;
}
