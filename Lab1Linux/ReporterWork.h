#ifndef REPORTER_WORK_H

#define REPORTER_WORK_H
#include <string>
#include <functional>

typedef std::function<double(double, double)> SalaryFunc;

class ReporterWork
{
public:
	static void PrintReport(const std::string &, const std::string &, double, const SalaryFunc &);
};

#endif