#include <iostream>
#include <functional>
#include "Client.h"
#include "ClientException.h"
#include "../Server/Employee.h"
#include "../Server/InputOutputException.h"

void ExitIfExceptionCaught(const std::function<void()> &function)
{
    try
    {
        function();
    }
    catch (const ClientException &ex)
    {
        std::cerr << "Something went wrong with the client!\n"
                  << "Message: " << ex.what();
        std::exit(-2);
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

enum class MenuChoice
{
    GET_RECORD = 1,
    UPDATE_RECORD,
    EXIT
};

int main()
{
    Client &client = Client::GetInstance();
    std::cout << "Connecting to the server...\n";
    ExitIfExceptionCaught([&client]() { client.TryConnectToServer(); });
    std::cout << "Connection established!\n";

    MenuChoice choice = MenuChoice::GET_RECORD;
    int number;
    Employee employee;
    while (choice != MenuChoice::EXIT)
    {
        std::cout << "Type " << (int)MenuChoice::GET_RECORD
                  << ", if you want to look at an employee;\n"
                  << (int)MenuChoice::UPDATE_RECORD << ", "
                  << "if you want to update an employee;\n"
                  << (int)MenuChoice::EXIT << ", if you want to exit.\n";
        std::cin >> number;
        choice = (MenuChoice)number;

        switch (choice)
        {
        case MenuChoice::GET_RECORD:
        {
            std::cout << "Type the ID of an employee you wanna find: ";
            std::cin >> number;
            std::cin.ignore();
            std::cout << "Sending the query to the server...\n";
            ExitIfExceptionCaught([&employee, &client, number]() 
                                  { employee = client.GetEmployee(number, true); });
            number = employee.num;
            if (employee.num < 0)
                std::cout << "No employee to read!\n";
            else
            {
                std::cout << "The requested employee:\n";
                std::cout << employee << '\n';
            }
        }
        break;

        case MenuChoice::UPDATE_RECORD:
        {
            std::cout << "Type the ID of an employee you wanna update: ";
            std::cin >> number;
            std::cin.ignore();
            std::cout << "Sending the query to the server...\n";
            ExitIfExceptionCaught([&employee, &client, number]() 
                                  { employee = client.GetEmployee(number, false); });
            number = employee.num;
            if (employee.num < 0)
            {
                std::cout << "No employee to read!\n";
                break;
            }
            else
            {
                std::cout << "The requested employee:\n";
                std::cout << employee << '\n';
            }
            ReadEmployeeFromConsole(employee, false);
            std::cout << "Press any key to send the employee...";
            std::cin.ignore();
            std::cout << "Sending the query to the server...\n";
            ExitIfExceptionCaught([&client, &employee]() { client.WriteEmployee(employee); });
        }
        break;

        case MenuChoice::EXIT:
        {
            std::cout << "Sending the exit query...\n";
            ExitIfExceptionCaught([&client]() { client.SendExit(); });
        }
        break;
        }

        if (choice != MenuChoice::EXIT)
        {
            if (number >= 0)
            {
                std::cout << "Press any key to release the record...";
                std::cin.ignore();
                ExitIfExceptionCaught([&client, number]() { client.SendRelease(number); });
            }
        }
    }
}