#pragma once
#ifndef POBEGOS_LAB_5_SERVER_H

    #define POBEGOS_LAB_5_SERVER_H

    #include <unordered_map>
    #include <string>
    #include <iostream>
    #include <Windows.h>
    #include "RecordLabel.h"
    #include "Employee.h"
    #include "SharedBinaryFile.h"
    #include "CriticalSection.h"

    class Server
    {
    private:
        std::unordered_map<int, RecordLabel> recordMap;
        SharedBinaryFile *binaryFile;
        CriticalSection mapSection;
        CriticalSection logSection;

        Server();
        Server(const Server &) = default;
        Server(Server &&) = default;
        Server & operator= (const Server &) = default;
        Server & operator= (Server &&) = default;
        virtual ~Server();

    public:
        static Server & GetInstance();

        void OpenBinaryFile(const std::string &fileName);
        void AddEmployee(const Employee &employee);
        void PrintEmployees(std::ostream &str);
        Employee GetEmployee(int recordID);
        void WriteEmployee(const Employee &employee);
        void ChangeState(int recordID, RecordState stateToMove);
    };

#endif // POBEGOS_LAB_5_SERVER_H