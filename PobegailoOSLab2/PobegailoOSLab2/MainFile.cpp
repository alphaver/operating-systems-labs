#include "Runner.h"
#include <iostream>
#include <exception>

int main()
{
	Runner::ReadArray();
	std::cout << "Your array:\n";
	Runner::PrintArray();

	try
	{
		Runner::RunThreads();
	}
	catch (RunnerException &ex)
	{
		std::cerr << "An error with threads has occurred: " << ex.what();
		return ex.GetErrCode();
	}
	catch (std::exception &ex)
	{
		std::cerr << "An unexpected error has occurred: " << ex.what();
		return 0xABBA;
	}

	try
	{
		Runner::SubstituteMinMax();
	}
	catch (RunnerException &ex)
	{
		std::cerr << "An error with substituting has occurred: " << ex.what();
		return ex.GetErrCode();
	}
	catch (std::exception &ex)
	{
		std::cerr << "An unexpected error has occurred: " << ex.what();
		return 0xABBA;
	}

	std::cout << "The resulting array:\n";
	Runner::PrintArray();

	std::system("pause");
	return 0;
}