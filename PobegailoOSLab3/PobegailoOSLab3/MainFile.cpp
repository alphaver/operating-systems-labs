#include "Runner.h"
#include <iostream>

int main()
{
	std::cout << "Input the number of elements in the array: ";
	int arrayNum;
	std::cin >> arrayNum;
	Runner::CreateArray(arrayNum);

	// 'littleNiggers' here is just a reference to
	// Agatha Christie's famous novel 'Ten Little
	// Niggers' also known as 'And Then There Were 
	// None' and 'Ten Little Indians.'
	std::cout << "Input the number of markers: ";
	int littleNiggers, nigger;
	std::cin >> littleNiggers;
	Runner::CreateAndRunMarkers(littleNiggers);

	while (littleNiggers > 0)
	{
		Runner::WaitForMarkers();

		std::cout << "The array: ";
		Runner::PrintArray();

		std::cout << "Choose a Marker to kill: ";
		std::cin >> nigger;
		if (!Runner::TryKillMarker(nigger - 1))
			std::cout << "Couldn't kill Marker!";
		else
			--littleNiggers;

		std::cout << "The resulting array: ";
		Runner::PrintArray();

		Runner::ResumeMarkers();
	}

	Runner::Dispose();
}