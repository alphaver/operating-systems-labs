#include "ReporterWork.h"

int main(int argc, char **argv)
{
	ReporterWork::PrintReport(argv[1], argv[2], std::stod(argv[3]));
	return 0;
}