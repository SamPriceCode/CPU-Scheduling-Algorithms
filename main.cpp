#include "schedulers.h"

using namespace std;
using namespace sch;

int main() {
	int quantum[2] = { 5, 10 };

	//CPU_Sim(0);

	CPU_Sim_Q(0, quantum);

	return 0;
}