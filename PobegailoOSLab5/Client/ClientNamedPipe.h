#pragma once
#ifndef POBEGOS_LAB_5_CLIENTNAMEDPIPE_H

    #define POBEGOS_LAB_5_CLIENTNAMEDPIPE_H

    #include <string>   
    #include <Windows.h>
    #include "../Server/PipeMessage.h"

    class ClientNamedPipe
    {
    private:
        static const std::string PIPE_PREFIX;

        HANDLE hNamedPipe;
        std::string pipeName;

        ClientNamedPipe(const ClientNamedPipe &) = default;
        ClientNamedPipe(ClientNamedPipe &&) = default;
        ClientNamedPipe & operator= (const ClientNamedPipe &) = default;
        ClientNamedPipe & operator= (ClientNamedPipe &&) = default;

    public:
        ClientNamedPipe(const std::string &pipeName);
        virtual ~ClientNamedPipe();

        const std::string & GetPipeName() const;

        bool TryConnectToServer();
        void SendPipeMessage(const PipeMessage &pipeMessage);
        void ReceivePipeMessage(PipeMessage &pipeMessage);
        bool DisconnectFromServer();
    };

#endif // POBEGOS_LAB_5_CLIENTNAMEDPIPE_H