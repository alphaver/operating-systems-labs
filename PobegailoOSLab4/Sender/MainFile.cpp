#include <iostream>
#include <cctype>
#include "SenderWorker.h"
#include "../PobegailoOSLab4/SyscallException.h"

int main(int argc, char **argv)
{
    try
    {
        SenderWorker::InitializeSender();
    }
    catch (const SyscallException &ex)
    {
        std::cerr << "A syscall error has occurred: " << ex.what();
        SenderWorker::DisposeResources();
        return ex.GetErrorCode();
    }
    catch (const std::exception &ex)
    {
        std::cerr << "An unknown error has occurred: " << ex.what();
        SenderWorker::DisposeResources();
        return 0xFF;
    }

    try
    {
        SenderWorker::OpenBinaryFile(argv[0], argv[1]);
    }
    catch (const SyscallException &ex)
    {
        std::cerr << "A syscall error has occurred: " << ex.what();
        SenderWorker::DisposeResources();
        return ex.GetErrorCode();
    }
    catch (const std::exception &ex)
    {
        std::cerr << "An unknown error has occurred: " << ex.what();
        SenderWorker::DisposeResources();
        return 0xFF;
    }

    SenderWorker::SignalReceiver();

    std::string response = "M", message;
    while (std::toupper(response[0]) == 'M')
    {
        std::cout << "Type an 'M' to send the message or anything else to quit: ";
        std::getline(std::cin, response);
        if (response.length() == 1 && std::toupper(response[0]) == 'M')
        {
            std::cout << "Type the message to send: ";
            std::getline(std::cin, message);

            try
            {
                SenderWorker::SendMessageToReceiver(message);
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

    SenderWorker::DisposeResources();
}