#ifndef RUNNER_H
#define RUNNER_H

#include <string>
#include <exception>
#include <iostream>
#include <functional>

typedef unsigned long ulong;
typedef std::function<void()> ChildFunc;

class Runner
{
private:
    static void runProcess(const std::string &, const ChildFunc &);
public:
    static void RunCreator(const std::string &, int);
    static void RunReporter(const std::string &, const std::string &, double);
    static void PrintBinaryFile(std::ostream &, const std::string &);
    static void PrintReport(std::ostream &, const std::string &);
};

class RunnerException : public std::runtime_error
{
private:
	ulong errCode;
public:
	RunnerException(const std::string &message, ulong errCode)
		: std::runtime_error(message.c_str()), errCode(errCode) {}
	
	ulong GetErrCode()
	{
		return errCode;
	}
};

#endif