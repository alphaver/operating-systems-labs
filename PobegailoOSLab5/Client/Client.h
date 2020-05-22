#pragma once
#ifndef POBEGOS_LAB_5_CLIENT_H

    #define POBEGOS_LAB_5_CLIENT_H

    #include "ClientNamedPipe.h"
    #include "../Server/PipeMessage.h"
    #include "../Server/Employee.h"

    class Client
    {
    private:
        static const std::string PIPE_NAME_PREFIX;
        ClientNamedPipe *namedPipe;

        Client();
        Client(const Client &) = default;
        Client(Client &&) = default;
        Client & operator= (const Client &) = default;
        Client & operator= (Client &&) = default;
        virtual ~Client();

    public:
        static Client & GetInstance();

        bool TryConnectToServer();
        Employee GetEmployee(int ID, bool readOnly);
        void WriteEmployee(const Employee &newEmployee);
        void SendRelease(int ID);
        void SendExit();
        bool DisconnectFromServer();
    };

#endif // POBEGOS_LAB_5_CLIENT_H