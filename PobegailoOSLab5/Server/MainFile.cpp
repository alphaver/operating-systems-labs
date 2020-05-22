#include <iostream>
#include <string>
#include <functional>
#include "ClientServants.h"
#include "Employee.h"
#include "Server.h"
#include "InputOutputException.h"
#include "ServerException.h"

void ExitIfExceptionCaught(const std::function<void()> &function)
{
    try
    {
        function();
    }
    catch (const ServerException &ex)
    {
        std::cerr << "Something went wrong with the server!\n"
                  << "Message: " << ex.what()
                  << "Error code: " << ex.GetErrorCode();
        std::exit(ex.GetErrorCode());
    }
    catch (const InputOutputException &ex)
    {
        std::cerr << "Something went wrong during input/output!\n"
                  << "Message: " << ex.what() << "\n"
                  << "Error code: " << ex.GetErrorCode();
        std::exit(ex.GetErrorCode());
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Something unexpected has occurred!\n"
                  << "Message: " << ex.what();
        std::exit(-1);
    }
}

int main()
{
    Server &server = Server::GetInstance();
    
    std::cout << "Input the name of the binary file: ";
    std::string binaryFileName;
    std::getline(std::cin, binaryFileName);

    ExitIfExceptionCaught([&server, &binaryFileName]() { server.OpenBinaryFile(binaryFileName); });

    int numberOfRecords;
    std::cout << "Input the number of records: ";
    std::cin >> numberOfRecords;
    Employee employee;
    for (int i = 0; i < numberOfRecords; ++i)
    {
        std::cout << "\tInput the data for Employee #" << (i + 1) << ":\n";
        ReadEmployeeFromConsole(employee);
        ExitIfExceptionCaught([&server, &employee]() { server.AddEmployee(employee); });
    }
    
    std::cout << "The employees in the file " << binaryFileName << ":\n";
    ExitIfExceptionCaught([&server]() { server.PrintEmployees(std::cout); });

    int numberOfClients;
    std::cout << "Input the number of clients: ";
    std::cin >> numberOfClients;
    ClientServants servants(numberOfClients, "Client.exe");
    ExitIfExceptionCaught([&servants]() { servants.RunServants(); });

    std::cout << "Waiting while all servants have finished...\n";
    servants.WaitForServants();
    std::cout << "The employees in the modified file " << binaryFileName << ":\n";
    ExitIfExceptionCaught([&server]() { server.PrintEmployees(std::cout); });

    system("pause");
}