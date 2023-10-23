//Sam Price COP4610

#include "schedulers.h"
#include <iostream>

using namespace std;
using namespace sch;

int main() {
	//menu variables
	int choice = 0;
	int const MLFQ = 2, QUIT = 3; //FCFS and SJF are already constants 1 and 2

	//declare scheduler constants
		//these are used when calling for a specific scheduling algorithm
		//MLFQ is not an option, use CPU_Sim_Tq with the Quanta parameter
	int const FCFS = 0, SJF = 1;

	//initialize values for time quantum
	int quanta[2] = { 5, 10 };

	//uncomment any of the following function calls to test a scheduling algorithm

	cout << "CPU SIMULATION PROGRAM";

	while (choice != QUIT) {
		cout << "\n\nPlease choose an option: "
			<< "\n\t1. First Come First Serve"
			<< "\n\t2. Shortest Job First"
			<< "\n\t3. Multilevel Feedback Queue"
			<< "\n\t4. QUIT"
			<< "\nPlease choose an option: ";
		cin >> choice;

		choice = choice - 1;

		switch (choice) {
		case FCFS:
			//First Come First Serve
			CPU_Sim(FCFS);
			break;
		case SJF:
			//Shortest Job First
			CPU_Sim(SJF);
			break;
		case MLFQ:
			//Multilevel Feedback Queue
			CPU_Sim_Tq(FCFS, quanta);
			break;
		case QUIT:
			cout << "\n\n\nI'll schedule you in for another time.\n\n\n";
			break;
		default:
			cout << "Please choose a number 1 through 4.";
			break;
		}
	}

	return 0;
}