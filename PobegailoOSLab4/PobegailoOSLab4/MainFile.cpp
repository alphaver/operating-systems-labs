#include <iostream>
#include <cctype>
#include "ReceiverWorker.h"
#include "SyscallException.h"

int main()
{
    std::string binaryFileName;
    int numberOfRecords;
    std::cout << "Input the name of the file, please: ";
    std::getline(std::cin, binaryFileName);
    std::cout << "Input the number of records: ";
    std::cin >> numberOfRecords;
    try
    {
        ReceiverWorker::CreateEmptyBinaryFile(binaryFileName, numberOfRecords);
    }
    catch (const SyscallException &ex)
    {
        std::cerr << "A syscall error has occurred: " << ex.what();
        ReceiverWorker::DisposeResources();
        return ex.GetErrorCode();
    }
    catch (const std::exception &ex)
    {
        std::cerr << "An unknown error has occurred: " << ex.what();
        ReceiverWorker::DisposeResources();
        return 0xFF;
    }

    int numberOfSenders;
    std::cout << "Input the number of Senders: ";
    std::cin >> numberOfSenders;
    try
    {
        std::cout << "Waiting for Senders...\n";
        ReceiverWorker::RunAndWaitForSenders(numberOfSenders);
        std::cout << "All Senders are set and ready to go.\n";
    }
    catch (const SyscallException &ex)
    {
        std::cerr << "A syscall error has occurred: " << ex.what();
        ReceiverWorker::DisposeResources();
        return ex.GetErrorCode();
    }
    catch (const std::exception &ex)
    {
        std::cerr << "An unknown error has occurred: " << ex.what();
        ReceiverWorker::DisposeResources();
        return 0xFF;
    }

    std::string response = "M";
    std::cin.ignore();
    while (std::toupper(response[0]) == 'M')
    {
        std::cout << "Type an 'M' to read the message or anything else to quit: ";
        std::getline(std::cin, response);
        if (response.length() == 1 && std::toupper(response[0]) == 'M')
        {
            try
            {
                std::cout << ReceiverWorker::ReadMessage() << '\n';
            }
            catch (const SyscallException &ex)
            {
                std::cerr << "A syscall error has occurred: " << ex.what() << '\n';
            }
            catch (const std::exception &ex)
            {
                std::cerr << "An unexpected error has occurred: " << ex.what() << '\n';
            }
        }
    }

    ReceiverWorker::DisposeResources();
    return 0;
}