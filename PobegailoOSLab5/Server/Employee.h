#pragma once
#ifndef POBEGOS_LAB_5_EMPLOYEE_H

    #define POBEGOS_LAB_5_EMPLOYEE_H

    #include <iostream>
    
    struct Employee
    {
        int num;
        char name[10];
        double hours;
    };

    inline void ReadEmployeeFromConsole(Employee &employee, bool changeID = true)
    {
        if (changeID)
        {
            std::cout << "Input the employee's ID: ";
            std::cin >> employee.num;
            std::cin.ignore();
        }
        std::cout << "Input the name of the employee: ";
        std::cin.getline(employee.name, sizeof(employee.name));
        std::cout << "Input the hours of the employee: ";
        std::cin >> employee.hours;
        std::cin.ignore();
    }

    inline std::ostream & operator<< (std::ostream &str, const Employee &employee)
    {
        return str << "Employee " << employee.name
                   << ", ID: " << employee.num
                   << ", hours: " << employee.hours;
    }

#endif // POBEGOS_LAB_5_EMPLOYEE_H

