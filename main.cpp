#include "schedulers.h"

using namespace std;
using namespace sch;

int main() {
	//declare scheduler constants
	int const FCFS = 0, SJF = 1;

	//initialize values for time quantum
	int quantum[2] = { 5, 10 };

	//uncomment any of the following function calls to test scheduling algorithm

	//First Come First Serve
	CPU_Sim(FCFS);

	//Shortest Job First
	//CPU_Sim(SJF);

	//Multilevel Feedback Queue
	//CPU_Sim_Q(FCFS, quantum);

	return 0;
}