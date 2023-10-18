#ifndef SCHEDULERS_H
#define SCHEDULERS_H

#include <iomanip>
#include <vector>
#include <string>

using namespace std;

namespace sch {
	extern const int P1[15];
	extern const int P2[17];
	extern const int P3[17];
	extern const int P4[17];
	extern const int P5[19];
	extern const int P6[17];
	extern const int P7[15];
	extern const int P8[11];

	extern const int* PROGRAMS[8];

	extern const int NEW, READY, CPU, IO, TERMINATE;

	struct PCB {
		int number;
		const int* instructions;
		int state = NEW;
		int waittime = 0;
		int responsetime = 0;
		int counter = 0;
		int ioBurst = 0;
		bool accessed = false;
	};
	
	//First Come First Serve
	void FirstComeFirstServe();


	//General CPU commands
	void loadPrograms(vector<PCB>&);
	void addWait(vector<PCB>&);
	void addResponse(vector<PCB>&);

	void decreaseIO(vector<PCB>&);
	void dismissIO(vector<PCB>&, vector<PCB>&);
}

#endif