#ifndef SCHEDULERS_H
#define SCHEDULERS_H

#include <iomanip>
#include <vector>
#include <string>

using namespace std;

namespace sch {
	struct PCB {
		int number;
		const int* instructions;
		int length;
		int cputime = 0;
		int iotime = 0;
		int waittime = 0;
		int responsetime = 0;
		int counter = 0;
		int ioBurst = 0;
		int cpuBurst = 0;
		bool accessed = false;
	};
	
//Main simulation function


	void CPU_Sim(int);


	//Simulates a CPU and desired scheduling algorithm
	//Input Choice for Simulation:
	// 1. First Come First Serve
	// 2. Shortest Job First
	//Input for Time Quantum:
	//	
	void CPU_Sim_Q(int, int[2]);


//General simulation commands

	//load program P# arrays from array PROGRAMS
	void loadPrograms(vector<PCB>&);
	//add wait and response time to processes in Ready queue
	void addTime(vector<PCB>&);

	void checkReady(vector<PCB>&);

	//decrease given ioBurst time for all processes in IO queue
	void decreaseIO(vector<PCB>&);
	//input IO queue and Ready queue, returns all ended IO processes to Ready queue
	void dismissIO(vector<PCB>&, vector<PCB>&);
	//prints remaining IO burst time for all processes in IO queue
	void checkIO(vector<PCB>&);

	int longestIO(vector<PCB>&);

	//gets length of a given array
	int getInstructionLength(const int[], int);
	//prints struct variable length from a given PCB
	void processLengths(vector<PCB>&);

	//prints the waiting, response, and turnaround time for all given processes
	void showStats(vector<PCB>&, int, int);


//Scheduling Functions
	int selectSchedule(vector<PCB>&, int);
	int selectSchedule(vector<PCB>&, int, int);


	int FCFS();

	int SJF(vector<PCB>&);
}

#endif