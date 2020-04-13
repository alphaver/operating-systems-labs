#ifndef POBEGOS_LAB4_SYSCALLEXCEPTION_H

#define POBEGOS_LAB4_SYSCALLEXCEPTION_H

#include <string>
#include <exception>
#include <windows.h>

class SyscallException : public std::exception
{
private:
	DWORD errorCode;
public:
	SyscallException(const std::string &message, DWORD errorCode)
		: std::exception(message.c_str()), errorCode(errorCode) {}
	DWORD GetErrorCode() const
	{
		return errorCode;
	}
};

#endif // POBEGOS_LAB4_SYSCALLEXCEPTION_H