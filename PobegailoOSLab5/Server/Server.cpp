#include "Server.h"
#include "ServerException.h"

Server::Server()
    : binaryFile(nullptr) {}

Server::~Server()
{
    if (binaryFile != nullptr)
        delete binaryFile;
}

Server & Server::GetInstance()
{
    static Server instance;
    return instance;
}

void Server::OpenBinaryFile(const std::string &fileName)
{
    binaryFile = new SharedBinaryFile(fileName);
}

void Server::AddEmployee(const Employee &employee)
{
    recordMap[employee.num].offset = binaryFile->GetFileOffset();
    binaryFile->WriteToFile(&employee, sizeof(Employee));
}

void Server::PrintEmployees(std::ostream & str)
{
    Employee employee;
    for (const auto &pair : recordMap)
    {
        binaryFile->ReadFromFile(&employee, sizeof(employee), pair.second.offset);
        str << employee << '\n';
    }
}

Employee Server::GetEmployee(int recordID)
{
    Employee employee;
    std::uint32_t offset;

    mapSection.Enter();
    if (recordMap.find(recordID) == recordMap.end())
    {
        mapSection.Leave();
        throw ServerException("No such ID in the file!");
    }
    if (recordMap[recordID].state == RecordState::FREE)
    {
        mapSection.Leave();
        throw ServerException("Incorrect state of the record for reading!");
    }
    offset = recordMap[recordID].offset;
    mapSection.Leave();

    binaryFile->ReadFromFile(&employee, sizeof(employee), offset);
    return employee;
}

void Server::WriteEmployee(const Employee &employee)
{
    std::uint32_t offset;

    mapSection.Enter();
    if (recordMap.find(employee.num) == recordMap.end())
    {
        mapSection.Leave();
        throw ServerException("No such ID in the file!");
    }
    if (recordMap[employee.num].state != RecordState::WRITE)
    {
        mapSection.Leave();
        throw ServerException("Incorrect state of the record for writing!");
    }
    offset = recordMap[employee.num].offset;
    mapSection.Leave();

    binaryFile->WriteToFile(&employee, sizeof(employee), offset);
}

void Server::ChangeState(int recordID, RecordState stateToMove)
{
    mapSection.Enter();

    if (recordMap.find(recordID) == recordMap.end())
    {
        mapSection.Leave();
        throw ServerException("No such ID in the file!");
    }

    RecordLabel &label = recordMap[recordID];
    switch (stateToMove)
    {
    case RecordState::FREE:
        {
            --label.numberOfThreads;
            if (label.numberOfThreads == 0)
                label.state = RecordState::FREE;
        }
        break;

    case RecordState::READ:
        {
            while (label.state == RecordState::WRITE)
            {
                mapSection.Leave();
                Sleep(1);
                mapSection.Enter();
            }
            label.state = RecordState::READ;
            ++label.numberOfThreads;
        }
        break;

    case RecordState::WRITE:
        {
            while (label.state != RecordState::FREE)
            {
                mapSection.Leave();
                Sleep(1);
                mapSection.Enter();
            }
            label.state = RecordState::WRITE;
            label.numberOfThreads = 1;
        }
        break;
    }

    mapSection.Leave();
}