#include "schedulers.h"

using namespace std;
using namespace sch;

int main() {
	//initialize values for time quantum
	int quantum[2] = { 5, 10 };

	//uncomment any of the following function calls to test scheduling algorithm

	//First Come First Serve
	CPU_Sim(0);

	//Shortest Job First
	//CPU_Sim(1);

	//Multilevel Feedback Queue
	//CPU_Sim_Q(0, quantum);

	return 0;
}