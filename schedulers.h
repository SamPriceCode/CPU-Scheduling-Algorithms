#ifndef SCHEDULERS_H
#define SCHEDULERS_H

#include <iomanip>
#include <vector>
#include <string>

using namespace std;

namespace sch {
//Process Control Board Structure
	struct PCB {
	//Main Process Information
		
		//program number (ID)
		int number;
		//pointer to program array which contains instructions
		//call using intrstructions[#]
		const int* instructions;
		//program counter (which instruction to be executed)
		int counter = 0;
		//length of given program
		int length;

	//Process Execution Information
		
		//How much time is left in the current IO Burst
		int ioBurst = 0;
		//How much time is left in the current CPU Burst
		int cpuBurst = 0;
	
	//Accounting Information

		//Time spent executing CPU bursts
		int cputime = 0;
		//Time spent executing
		int iotime = 0;
		int waittime = 0;
		int responsetime = 0;
		bool accessed = false;
		
		

		
	};
	
//Main simulation function

	//Simulates a CPU and desired scheduling algorithm
	//Input Choice for Simulation:
	//	1. First Come First Serve
	//	2. Shortest Job First
	void CPU_Sim(int);


	//Simulates a CPU and desired scheduling algorithm
	//Input Choice for Simulation:
	//	1. First Come First Serve
	//	2. Shortest Job First
	//Input for Time Quantum:
	//	must be an int array with 2 items
	void CPU_Sim_Q(int, int[2]);


//General Simulation Commands

	//load program P# arrays from array PROGRAMS
	void loadPrograms(vector<PCB>&);

	//add wait and response time to processes in Ready Queue
	void addTime(vector<PCB>&);

	//prints all current proc
	void checkReady(vector<PCB>&);

	//decrease given ioBurst time for all processes in IO Queue
	void decreaseIO(vector<PCB>&);

	//returns any finished process in the IO Queue to the Ready Queue
	void dismissIO(vector<PCB>&, vector<PCB>&, float&);

	//prints remaining IO burst time for all processes in IO Queue
	void checkIO(vector<PCB>&);

	//finds the process with the longest current remaining IO burst
	int longestIO(vector<PCB>&);

	//gets length of a process's instructions
	int getInstructionLength(const int[], int);

	//prints struct variable length from a given PCB
	void processLengths(vector<PCB>&);

	//prints the waiting, response, and turnaround time for all given processes
	void showStats(vector<PCB>&, int, int);


//Scheduling Functions

	//input determines scheduling algorithm, 1 = FCFS, 2 = SJF
	//returns an integer value corresponding to a position in the ready list
	int selectSchedule(vector<PCB>&, int, float&);

	//First Come First Serve Scheduling Algorithm
	// - return first position the ready queue
	int FCFS(float&);

	//Shortest Job First Scheduling Algorithm
	// - finds the process with the lowest burst length
	// - returns it's position the ready queue
	int SJF(vector<PCB>&, float&);
}

#endif