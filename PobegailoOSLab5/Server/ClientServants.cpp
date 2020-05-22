#include "ClientServants.h"
#include "ServerException.h"
#include "Server.h"

const std::string ClientServants::Servant::PIPE_NAME_PREFIX = "Lab5ClientPipe";

DWORD WINAPI ClientServants::Servant::threadWrapperFunction(LPVOID lpServant)
{
    ClientServants::Servant *servant = reinterpret_cast<ClientServants::Servant *>(lpServant);
    return servant->servantThreadFunction(&Server::GetInstance());
}

DWORD WINAPI ClientServants::Servant::servantThreadFunction(LPVOID lpServer)
{
    if (!namedPipe->TryConnectToClient())
        return 1;

    Server &server = *reinterpret_cast<Server *>(lpServer);

    PipeMessage incoming = { PipeMessageType::EXIT };
    PipeMessage outgoing = { PipeMessageType::SUCCESS };

    try
    {
        namedPipe->ReceivePipeMessage(incoming);
    }
    catch (const std::exception &)
    {
        return 2;
    }
    lastID = -1;
    while (incoming.type != PipeMessageType::EXIT)
    {
        try
        {
            switch (incoming.type)
            {
            case PipeMessageType::GET_EMPLOYEE:
            {
                int ID;
                bool readOnly;
                memcpy(&ID, incoming.buffer, sizeof(int));
                memcpy(&readOnly, incoming.buffer + sizeof(int), sizeof(bool));
                lastID = ID;
                Employee employee;
                try
                {
                    server.ChangeState(ID, readOnly ? RecordState::READ : RecordState::WRITE);
                    employee = server.GetEmployee(ID);
                }
                catch (const ServerException &)
                {
                    employee.num = -1;
                }
                memcpy(outgoing.buffer, &employee, sizeof(employee));
                outgoing.type = PipeMessageType::SEND_EMPLOYEE;
            }
            break;

            case PipeMessageType::WRITE_EMPLOYEE:
            {
                Employee employee;
                memcpy(&employee, incoming.buffer, sizeof(employee));
                lastID = employee.num;
                server.WriteEmployee(employee);
                outgoing.type = PipeMessageType::SUCCESS;
            }
            break;

            case PipeMessageType::RELEASE:
            {
                int ID;
                memcpy(&ID, incoming.buffer, sizeof(int));
                lastID = ID;
                server.ChangeState(ID, RecordState::FREE);
                outgoing.type = PipeMessageType::SUCCESS;
                lastID = -1;
            }
            break;

            default:
                outgoing.type = PipeMessageType::FAILURE;
            }

            if (incoming.type != PipeMessageType::EXIT)
            {
                namedPipe->SendPipeMessage(outgoing);
                namedPipe->ReceivePipeMessage(incoming);
            }
        }
        catch (const std::exception &)
        {
            try
            {
                if (lastID != -1)
                    server.ChangeState(lastID, RecordState::FREE);
            }
            catch (const std::exception &) {}

            outgoing.type = PipeMessageType::FAILURE;
            try
            {
                namedPipe->SendPipeMessage(outgoing);
            }
            catch (const std::exception &) {}

            return 2;
        }
    }

    return 0;
}

ClientServants::Servant::Servant(const std::string &clientProcessName)
    : namedPipe(nullptr)
{
    ZeroMemory(&siInfo, sizeof(siInfo));
    siInfo.cb = sizeof(siInfo);
    ZeroMemory(&piClient, sizeof(piClient));

    BOOL processResult = CreateProcess
                         (nullptr,
                          const_cast<char *>(clientProcessName.c_str()),
                          nullptr,
                          nullptr,
                          FALSE,
                          CREATE_NEW_CONSOLE | CREATE_SUSPENDED,
                          nullptr,
                          nullptr,
                          &siInfo,
                          &piClient);
    if (!processResult)
        throw new ServerException("The servant couldn't create the process!");

    hServantThread = CreateThread
                     (nullptr,
                      0,
                      threadWrapperFunction,
                      this,
                      CREATE_SUSPENDED,
                      nullptr);
    if (hServantThread == nullptr)
        throw new ServerException("The servant couldn't create the serving thread!");

    namedPipe = new ServerNamedPipe(PIPE_NAME_PREFIX + "#" + std::to_string(piClient.dwProcessId));
}

ClientServants::Servant::~Servant()
{
    CloseHandle(piClient.hProcess);
    CloseHandle(piClient.hThread);
    CloseHandle(hServantThread);
    if (namedPipe != nullptr)
        delete namedPipe;
}

void ClientServants::Servant::RunServant()
{
    ResumeThread(hServantThread);
    ResumeThread(piClient.hThread);
}

HANDLE ClientServants::Servant::GetThreadHandle() const
{
    return hServantThread;
}

ClientServants::ClientServants(int numberOfServants, const std::string &clientProcess)
    : numberOfServants(numberOfServants)
{
    servants = new Servant *[numberOfServants];
    servantThreadHandles = new HANDLE[numberOfServants];
    for (int i = 0; i < numberOfServants; ++i)
    {
        servants[i] = new Servant(clientProcess);
        servantThreadHandles[i] = servants[i]->GetThreadHandle();
    }
}

ClientServants::~ClientServants()
{
    for (int i = 0; i < numberOfServants; ++i)
        delete servants[i];
    delete servants;
    delete servantThreadHandles;
}

int ClientServants::GetNumberOfServants() const
{
    return numberOfServants;
}

void ClientServants::RunServants()
{
    for (int i = 0; i < numberOfServants; ++i)
        servants[i]->RunServant();
}

void ClientServants::WaitForServants()
{
    WaitForMultipleObjects(numberOfServants, servantThreadHandles, TRUE, INFINITE);
}
