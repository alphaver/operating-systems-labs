#pragma once
#ifndef POBEGOS_LAB_5_SERVEREXCEPTION_H

    #define POBEGOS_LAB_5_SERVEREXCEPTION_H

    #include <string>
    #include <exception>
    #include <Windows.h>

    class ServerException: public std::exception
    {
    private:
        std::uint32_t errorCode;

    public:
        ServerException(const std::string &message)
            : std::exception(message.c_str()), errorCode(GetLastError()) {}
        virtual ~ServerException() {}

        std::uint32_t GetErrorCode() const 
        {
            return errorCode;
        }
    };

#endif // POBEGOS_LAB_5_SERVEREXCEPTION_H

