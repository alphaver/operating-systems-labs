#include "CreatorWork.h"
#include "Employee.h"
#include <iostream>
#include <fstream>

void inputEmployee(Employee &employee)
{
	std::cout << "Input the ID of the employee: ";
	std::cin >> employee.num;
	std::cin.ignore();
	std::cout << "Input the name of the employee (<= 9 chars): ";
	std::cin.getline(employee.name, 10);
	std::cout << "Input the working hours of the employee: ";
	std::cin >> employee.hours;
	std::cin.ignore();
}

void CreatorWork::FillBinaryFile
	(const std::string &fileName,
	 int numberOfRecords)
{
	std::ofstream outFile(fileName, std::ios::binary);

	Employee slave;
	for (int i = 0; i < numberOfRecords; ++i)
	{
		std::cout << "Input the employee # " << (i + 1) << "'s data:\n";
		inputEmployee(slave);
		outFile.write(reinterpret_cast<char*>(&slave), sizeof(slave));
	}

	outFile.close();
}