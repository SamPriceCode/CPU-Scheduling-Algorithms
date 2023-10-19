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
		int waittime = 0;
		int responsetime = 0;
		int counter = 0;
		int ioBurst = 0;
		bool accessed = false;
	};
	
	//First Come First Serve
		void FirstComeFirstServe();


	//General simulation commands
		//load program P# arrays from array PROGRAMS
		void loadPrograms(vector<PCB>&);
		//add wait and response time to processes in Ready queue
		void addTime(vector<PCB>&);

		//decrease given ioBurst time for all processes in IO queue
		void decreaseIO(vector<PCB>&);
		//input IO queue and Ready queue, returns all ended IO processes to Ready queue
		void dismissIO(vector<PCB>&, vector<PCB>&);
		//prints remaining IO burst time for all processes in IO queue
		void checkIO(vector<PCB>&);

		int longestIO(vector<PCB>&);

		//gets length of a given array
		int getInstructionLength(const int process[], int);
		//prints struct variable length from a given PCB
		void processLengths(vector<PCB>&);

		//prints the waiting, response, and turnaround time for all given processes
		void showStats(vector<PCB>&, int);
}

#endif