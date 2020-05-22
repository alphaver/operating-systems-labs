#pragma once
#ifndef POBEGOS_LAB_5_CLIENTSERVANTS_H

    #define POBEGOS_LAB_5_CLIENTSERVANTS_H

    #include <iostream>
    #include <Windows.h>
    #include "ServerNamedPipe.h"

    class ClientServants
    {
    private:
        class Servant
        {
        private:
            static const std::string PIPE_NAME_PREFIX;
            HANDLE hServantThread;
            STARTUPINFO siInfo;
            PROCESS_INFORMATION piClient;
            ServerNamedPipe *namedPipe;
            int lastID;

            DWORD WINAPI servantThreadFunction(LPVOID lpServer);
            static DWORD WINAPI threadWrapperFunction(LPVOID lpServant);

            Servant(const Servant &) = default;
            Servant(Servant &&) = default;
            Servant & operator= (const Servant &) = default;
            Servant & operator= (Servant &&) = default;

        public:
            Servant(const std::string &clientProcessName);
            virtual ~Servant();

            void RunServant();
            HANDLE GetThreadHandle() const;
        };

        Servant **servants;
        HANDLE *servantThreadHandles;
        int numberOfServants;

    public:
        ClientServants(int numberOfServants, const std::string &clientProcess);
        virtual ~ClientServants();

        int GetNumberOfServants() const;
        void RunServants();
        void WaitForServants();
    };

#endif // POBEGOS_LAB_5_CLIENTSERVANTS_H
