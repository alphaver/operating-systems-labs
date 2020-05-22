#include <functional>
#include "Client.h"
#include "ClientException.h"

const std::string Client::PIPE_NAME_PREFIX = "Lab5ClientPipe";

Client::Client()
    : namedPipe(nullptr) {}

Client::~Client()
{
    if (namedPipe != nullptr)
        delete namedPipe;
}

Client & Client::GetInstance()
{
    static Client instance;
    return instance;
}

bool Client::TryConnectToServer()
{
    namedPipe = new ClientNamedPipe(PIPE_NAME_PREFIX + "#" + std::to_string(GetCurrentProcessId()));
    return namedPipe->TryConnectToServer();
}

Employee Client::GetEmployee(int ID, bool readOnly)
{
    Employee employee;
    PipeMessage incoming = { PipeMessageType::SUCCESS };
    PipeMessage outgoing = { PipeMessageType::GET_EMPLOYEE };
    memcpy(outgoing.buffer, &ID, sizeof(int));
    memcpy(outgoing.buffer + sizeof(int), &readOnly, sizeof(bool));
    namedPipe->SendPipeMessage(outgoing);

    namedPipe->ReceivePipeMessage(incoming);
    if (incoming.type != PipeMessageType::SEND_EMPLOYEE)
        throw ClientException("Incorrect message arrived through the pipe!");
    memcpy(&employee, incoming.buffer, sizeof(employee));

    return employee;
}

void Client::WriteEmployee(const Employee &newEmployee)
{
    PipeMessage incoming = { PipeMessageType::SUCCESS };
    PipeMessage outgoing = { PipeMessageType::WRITE_EMPLOYEE };
    memcpy(outgoing.buffer, &newEmployee, sizeof(newEmployee));
    namedPipe->SendPipeMessage(outgoing);

    namedPipe->ReceivePipeMessage(incoming);
    if (incoming.type != PipeMessageType::SUCCESS)
        throw ClientException("Writing operation unsuccessful!");
}

void Client::SendRelease(int ID)
{
    PipeMessage incoming = { PipeMessageType::SUCCESS };
    PipeMessage outgoing = { PipeMessageType::RELEASE };
    memcpy(outgoing.buffer, &ID, sizeof(int));
    namedPipe->SendPipeMessage(outgoing);

    namedPipe->ReceivePipeMessage(incoming);
    if (incoming.type != PipeMessageType::SUCCESS)
        throw ClientException("Releasing the record is unsuccessful!");
}

void Client::SendExit()
{
    PipeMessage outgoing = { PipeMessageType::EXIT };
    namedPipe->SendPipeMessage(outgoing);
}

bool Client::DisconnectFromServer()
{
    return namedPipe->DisconnectFromServer();
}
