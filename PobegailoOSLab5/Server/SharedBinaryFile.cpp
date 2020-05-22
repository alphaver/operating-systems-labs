#include "SharedBinaryFile.h"
#include "InputOutputException.h"

void SharedBinaryFile::setFilePointer(std::uint32_t offset)
{
    if (SetFilePointer(hWindowsFile, offset, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
        throw InputOutputException("Can't move file pointer in file " + fileName + "!");
}

std::uint32_t SharedBinaryFile::getFilePointer() const
{
    std::uint32_t result = SetFilePointer(hWindowsFile, 0, nullptr, FILE_CURRENT);
    if (result == INVALID_SET_FILE_POINTER)
        throw InputOutputException("Can't get file pointer position in file " + fileName + "!");
    return result;
}

SharedBinaryFile::SharedBinaryFile(const std::string &fileName)
    : fileName(fileName)
{
    hWindowsFile = CreateFile
                   (fileName.c_str(),
                    GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                    nullptr,
                    OPEN_ALWAYS,
                    FILE_FLAG_RANDOM_ACCESS,
                    nullptr);
    if (hWindowsFile == INVALID_HANDLE_VALUE)
        throw InputOutputException("Can't open file " + fileName + "!");
}

SharedBinaryFile::~SharedBinaryFile()
{
    if (hWindowsFile != nullptr && hWindowsFile != INVALID_HANDLE_VALUE)
        CloseHandle(hWindowsFile);
}

const std::string & SharedBinaryFile::GetFileName() const
{
    return fileName;
}

std::uint32_t SharedBinaryFile::GetFileOffset() const
{
    return getFilePointer();
}

void SharedBinaryFile::ReadFromFile(void *buffer, std::uint32_t bytesToRead, std::uint32_t offset)
{
    DWORD bytesRead;
    if (offset != NO_OFFSET)
        setFilePointer(offset);
    if (!ReadFile(hWindowsFile, buffer, bytesToRead, &bytesRead, nullptr))
        throw InputOutputException("Can't read from file " + fileName + "!");
    if (bytesRead == 0)
        throw InputOutputException("The end of file " + fileName + " was reached!");
}

void SharedBinaryFile::WriteToFile(const void *buffer, std::uint32_t bytesToWrite, std::uint32_t offset)
{
    if (offset != NO_OFFSET)
        setFilePointer(offset);
    if (!WriteFile(hWindowsFile, buffer, bytesToWrite, nullptr, nullptr))
        throw InputOutputException("Can't write to file " + fileName + "!");
}