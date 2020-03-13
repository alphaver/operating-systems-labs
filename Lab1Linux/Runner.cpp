#include "Runner.h"
#include "Employee.h"
#include "CreatorWork.h"
#include "ReporterWork.h"
#include <unistd.h>
#include <sys/wait.h>
#include <cerrno>
#include <fstream>
#include <iomanip>

void Runner::runProcess
	(const std::string &processName,
	 const ChildFunc &childFunc)
{
	pid_t processHandle = fork();
	if (processHandle < 0)
	{
		std::string errMessage = "An error with " + processName + 
								 " has occurred, error code = " + 
								 std::to_string(errno);
		throw RunnerException(errMessage, errno);
	}
	else if (processHandle > 0)
	{
		int status;
		wait(&status);
		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
			throw RunnerException(processName + " didn't exit normally!", -1);
	}	
	else
	{
		childFunc();
		exit(0);
	}
}

void Runner::RunCreator
	(const std::string &fileName, 
	 int recordsAmount)
{
	runProcess("Creator",
			   [&] ()
			   { CreatorWork::FillBinaryFile(fileName, 
			   								 recordsAmount); } );
}

void Runner::RunReporter
	(const std::string &binFileName, 
	 const std::string &reportFileName, 
	 double hourlyRate)
{
	runProcess("Reporter",
			   [&] ()
			   { ReporterWork::PrintReport(binFileName, 
			   							   reportFileName, 
			   							   hourlyRate,
			   							   [] (double hours, double rate)
                                            {return hours * rate; }); });
}

void Runner::PrintBinaryFile
	(std::ostream &str,
 	 const std::string &binFileName)
{
	std::ifstream fileToRead(binFileName, std::ios::binary);
	str << std::setw(10) << "ID"
		<< std::setw(11) << "Name"
		<< std::setw(11) << "Hours" << '\n';
	Employee slave;
	while (fileToRead.read(reinterpret_cast<char*>(&slave), sizeof(slave)))
	{
		str << std::setw(10) << slave.num
			<< std::setw(11) << slave.name
			<< std::setw(11) << slave.hours << '\n';
	}
	fileToRead.close();
}

void Runner::PrintReport
	(std::ostream &str,
	 const std::string &repFileName)
{
	std::string textLine;
	std::ifstream fileToRead(repFileName);
	while (std::getline(fileToRead, textLine))
		str << textLine << '\n';
	fileToRead.close();
}