#pragma once
#include <string>
#include <exception>
#include <iostream>

typedef unsigned long ulong;

class Runner
{
private:
	static void runProcess(const std::string &, const std::string &);
public:
	static const std::string ExecuteFolder;
	static void RunCreator(const std::string &, int);
	static void RunReporter(const std::string &, const std::string &, double);
	static void PrintBinaryFile(std::ostream &, const std::string &);
	static void PrintReport(std::ostream &, const std::string &);
};

class RunnerException : public std::exception
{
private:
	ulong errCode;
public:
	RunnerException(const std::string &message, ulong errCode)
		: std::exception(message.c_str()), errCode(errCode) {}
	
	ulong GetErrCode()
	{
		return errCode;
	}
};