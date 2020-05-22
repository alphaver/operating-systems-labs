#pragma once
#ifndef POBEGOS_LAB_5_CLIENTEXCEPTION_H

    #define POBEGOS_LAB_5_CLIENTEXCEPTION_H

    #include <string>
    #include <exception>
    #include <Windows.h>

    class ClientException : public std::exception
    {
    public:
        ClientException(const std::string &message)
            : std::exception(message.c_str()) {}
        virtual ~ClientException() {}
    };

#endif // POBEGOS_LAB_5_CLIENTEXCEPTION_H

