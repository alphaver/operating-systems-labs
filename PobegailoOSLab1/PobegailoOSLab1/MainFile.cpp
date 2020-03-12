#include <iostream>
#include <string>
#include "Runner.h"
#include "Employee.h"

int main(int argc, char **argv)
{
	std::cout << "Input the name of the binary file: ";
	std::string binFileName;
	std::getline(std::cin, binFileName);
	std::cout << "Input the number of records: ";
	int recordsAmount;
	std::cin >> recordsAmount;
	std::cin.ignore();
	
	std::cout << "Running Creator...\n";
	try
	{
		Runner::RunCreator(binFileName, recordsAmount);
	}
	catch (RunnerException &ex)
	{
		std::cerr << "Something went wrong while running Creator: " << ex.what();
		return ex.GetErrCode();
	}
	catch (std::exception &ex)
	{
		std::cerr << "Something unexpected has occured: " << ex.what();
		return -1;
	}

	std::cout << "The employees in \"" << binFileName << "\":\n";
	Runner::PrintBinaryFile(std::cout, binFileName);

	std::cout << "Input the name of the report file: ";
	std::string repFileName;
	std::getline(std::cin, repFileName);
	std::cout << "Input the hourly rate: ";
	double hourlyRate;
	std::cin >> hourlyRate;

	std::cout << "Running Reporter...\n";
	try
	{
		Runner::RunReporter(binFileName, repFileName, hourlyRate);
	}
	catch (RunnerException &ex)
	{
		std::cerr << "Something went wrong while running Reporter: " << ex.what();
		return ex.GetErrCode();
	}
	catch (std::exception &ex)
	{
		std::cerr << "Something unexpected has occured: " << ex.what();
		return -1;
	}

	std::cout << "The report:\n";
	Runner::PrintReport(std::cout, repFileName);

	return 0;
}