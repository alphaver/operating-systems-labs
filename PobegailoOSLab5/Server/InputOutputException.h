#pragma once
#ifndef POBEGOS_LAB_5_FILEEXCEPTION_H

    #define POBEGOS_LAB_5_FILEEXCEPTION_H

    #include <string>
    #include <exception>
    #include <Windows.h>

    class InputOutputException: public std::exception
    {
    private:
        std::uint32_t errorCode;
    public:
        InputOutputException(const std::string &message)
            : std::exception(message.c_str()), errorCode(GetLastError()) {}
        virtual ~InputOutputException() {}

        std::uint32_t GetErrorCode() const
        {
            return errorCode;
        }
    };

#endif // POBEGOS_LAB_5_FILEEXCEPTION_H

