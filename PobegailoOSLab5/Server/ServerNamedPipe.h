#pragma once
#ifndef POBEGOS_LAB_5_SERVERNAMEDPIPE_H

    #define POBEGOS_LAB_5_SERVERNAMEDPIPE_H

    #include <string>   
    #include <Windows.h>
    #include "PipeMessage.h"

    class ServerNamedPipe
    {
    private:
        static const std::string PIPE_PREFIX;
        static const std::uint32_t DEFAULT_PIPE_TIMEOUT = 5000;

        HANDLE hNamedPipe;
        std::string pipeName;
        std::uint32_t timeout;
        OVERLAPPED overlapped;

        ServerNamedPipe(const ServerNamedPipe &) = default;
        ServerNamedPipe(ServerNamedPipe &&) = default;
        ServerNamedPipe & operator= (const ServerNamedPipe &) = default;
        ServerNamedPipe & operator= (ServerNamedPipe &&) = default;

    public:
        ServerNamedPipe(const std::string &pipeName, std::uint32_t timeout = DEFAULT_PIPE_TIMEOUT);
        virtual ~ServerNamedPipe();

        const std::string & GetPipeName() const;

        bool TryConnectToClient();
        void SendPipeMessage(const PipeMessage &pipeMessage);
        void ReceivePipeMessage(PipeMessage &pipeMessage);
        bool DisconnectFromClient();
    };

#endif // POBEGOS_LAB_5_NAMEDPIPE_H