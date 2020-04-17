#include "Runner.h"
#include <iostream>

int main()
{
	std::cout << "Input the number of elements in the array: ";
	int arrayNum;
	std::cin >> arrayNum;
	Runner::CreateArray(arrayNum);

	std::cout << "Input the number of markers: ";
	int markers, currentMarker;
	std::cin >> markers;
	Runner::CreateAndRunMarkers(markers);

	while (markers > 0)
	{
		Runner::WaitForMarkers();

		std::cout << "The array: ";
		Runner::PrintArray();

		std::cout << "Choose a Marker to kill: ";
		std::cin >> currentMarker;
		if (!Runner::TryKillMarker(currentMarker - 1))
			std::cout << "Couldn't kill Marker!";
		else
			--markers;

		std::cout << "The resulting array: ";
		Runner::PrintArray();

		Runner::ResumeMarkers();
	}

	Runner::Dispose();
}