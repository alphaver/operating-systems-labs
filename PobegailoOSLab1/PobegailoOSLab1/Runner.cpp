#include "Runner.h"
#include "Employee.h"
#include <Windows.h>
#include <fstream>
#include <iomanip>

const std::string Runner::ExecuteFolder = 
	R"(C:\Users\Xiaomi\source\repos\PobegailoOSLab1\Debug\)";

void Runner::runProcess
	(const std::string &processName,
	 const std::string &cmdArgs)
{
	std::string cmdLine = ExecuteFolder + processName + " " + cmdArgs;

	STARTUPINFOA startupInfo;
	ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
	PROCESS_INFORMATION processInfo;
	ZeroMemory(&processInfo, sizeof(processInfo));

	if (!CreateProcessA
		(NULL,
		 (char *)cmdLine.c_str(),
		 NULL,
		 NULL,
		 FALSE,
		 CREATE_NEW_CONSOLE,
		 NULL,
		 NULL,
		 &startupInfo,
		 &processInfo))
	{
		std::string errMessage = "An error with " + processName + " has occurred, error code = ";
		DWORD lastErrCode = GetLastError();

		throw RunnerException(errMessage + std::to_string(lastErrCode), lastErrCode);
	}

	WaitForSingleObject(processInfo.hProcess, INFINITE);

	DWORD exitCode;
	GetExitCodeProcess(processInfo.hProcess, &exitCode);
	if (!exitCode)
	{
		std::string errMessage = processName + " exited with the code = ";
		throw RunnerException(errMessage + std::to_string(exitCode), exitCode);
	}
	
	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread);
}

void Runner::RunCreator
	(const std::string &fileName, 
	 int recordsAmount)
{
	runProcess("Creator.exe", 
			   '\"' + fileName + "\" " + std::to_string(recordsAmount));
}

void Runner::RunReporter
	(const std::string &binFileName, 
	 const std::string &reportFileName, 
	 double hourlyRate)
{
	runProcess("Reporter.exe",
			   '\"' + binFileName + "\" \"" + reportFileName + "\" " +
			   std::to_string(hourlyRate));
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