#pragma once
#ifndef POBEGOS_LAB_5_SHAREDBINARYFILE_H

    #define POBEGOS_LAB_5_SHAREDBINARYFILE_H
    
    #include <string>
    #include <Windows.h>

    class SharedBinaryFile
    {
    private:
        HANDLE hWindowsFile;
        std::string fileName;

        void setFilePointer(std::uint32_t offset);
        std::uint32_t getFilePointer() const;

    public:
        static const std::uint32_t NO_OFFSET = -1;

        SharedBinaryFile(const std::string &fileName);
        virtual ~SharedBinaryFile();

        const std::string & GetFileName() const;
        std::uint32_t GetFileOffset() const;
        
        void ReadFromFile(void *buffer, std::uint32_t bytesToRead, std::uint32_t offset = NO_OFFSET);
        void WriteToFile(const void *buffer, std::uint32_t bytesToWrite, std::uint32_t offset = NO_OFFSET);
    };

#endif // POBEGOS_LAB_5_SHAREDBINARYFILE_H