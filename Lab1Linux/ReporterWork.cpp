#include "ReporterWork.h"
#include "Employee.h"
#include <iostream>
#include <fstream>
#include <iomanip>

void ReporterWork::PrintReport
	(const std::string &binFileName,
	 const std::string &repFileName,
	 double hourlyRate,
	 const SalaryFunc &salaryFunc)
{
	std::ifstream binFile(binFileName, std::ios::binary);
	std::ofstream reportFile(repFileName);

	reportFile << "A report on the file \"" << binFileName << "\"\n";
	reportFile << std::setw(10) << "ID"
			   << std::setw(11) << "Name"
			   << std::setw(11) << "Hours"
			   << std::setw(11) << "Salary" << '\n';
	Employee slave;
	while (binFile.read(reinterpret_cast<char*>(&slave), sizeof(slave)))
	{
		reportFile << std::setw(10) << slave.num
				   << std::setw(11) << slave.name
				   << std::setw(11) << slave.hours 
				   << std::setw(11) << salaryFunc(slave.hours, hourlyRate) << '\n';
	}

	binFile.close();
	reportFile.close();
}