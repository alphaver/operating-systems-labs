#include "CreatorWork.h"

int main(int argc, char **argv)
{
	CreatorWork::FillBinaryFile(argv[1], std::atoi(argv[2]));
	return 0;
}