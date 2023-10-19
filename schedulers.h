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
		int state = NEW;
		int waittime = 0;
		int responsetime = 0;
		int counter = 0;
		int ioBurst = 0;
		bool accessed = false;
	};
	
	//First Come First Serve
		void FirstComeFirstServe();


	//General simulation commands
		void loadPrograms(vector<PCB>&);
		void addTime(vector<PCB>&);

		void decreaseIO(vector<PCB>&);
		//input ioQ and readyQ, returns total dismissals
		void dismissIO(vector<PCB>&, vector<PCB>&);
		void checkIO(vector<PCB>&);

		int getInstructionLength(const int process[], int);
		void processLengths(vector<PCB>&);
}

#endif