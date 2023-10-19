#include "schedulers.h"
#include <iostream>
#include <vector>

using namespace std;

//constants
namespace sch {
	const int P1[] = { 5, 27, 3, 31, 5, 43, 4, 18, 6, 22, 4, 26, 3, 24, 4 };
	const int P2[] = { 4, 48, 5, 44, 7, 42, 12, 37, 9, 76, 4, 41, 9, 31, 7, 43, 8 };
	const int P3[] = { 8, 33, 12, 41, 18, 65, 14, 21, 4, 61, 15, 18, 14, 26, 5, 31, 6 };
	const int P4[] = { 3, 35, 4, 41, 5, 45, 3, 51, 4, 61, 5, 54, 6, 82, 5, 77, 3 };
	const int P5[] = { 16, 24, 17, 21, 5, 36, 16, 26, 7, 31, 13, 28, 11, 21, 6, 13, 3, 11, 4 };
	const int P6[] = { 11, 22, 4, 8, 5, 10, 6, 12, 7, 14, 9, 18, 12, 24, 15, 30, 8 };
	const int P7[] = { 14, 46, 17, 41, 11, 42, 15, 21, 4, 32, 7, 19, 16, 33, 10 };
	const int P8[] = { 4, 14, 5, 33, 6, 51, 14, 73, 16, 87, 6 };

	const int* PROGRAMS[] = { P1, P2, P3, P4, P5, P6, P7, P8 };

	const int NEW = 0, READY = 1, CPU = 2, IO = 3, TERMINATE = 4;

}

//simulation functions
namespace sch{
	void loadPrograms(vector<PCB>& readyQ) {
		for (int i = 0; i < 8; i++) {
			PCB process;
			process.number = i;
			process.instructions = PROGRAMS[i];
			process.length = getInstructionLength(PROGRAMS[i], process.number);
			readyQ.push_back(process);
		}

		cout << "\nPrograms Loaded Sucessfully";
	}

	void addTime(vector<PCB>& readyQ) {
		for (int i = 0; i < readyQ.size(); i++) {
			readyQ[i].waittime++;
			if (readyQ[i].accessed == false) {
				readyQ[i].responsetime++;
			}
		}
	}

	void decreaseIO(vector<PCB>& ioQ) {
		for (int i = 0; i < ioQ.size(); i++) {
			ioQ[i].ioBurst--;
		}

	}

	void dismissIO(vector<PCB>& ioQ, vector<PCB>& readyQ) {
		//cout << "\n\t\t\t\tDismiss IO & sizeL:" << ioQ.size() << endl;
		for (int i = 0; i < ioQ.size(); i++) {
			//cout << "\n\t\t Process " << ioQ[i].number << " I/O burst time: " << ioQ[i].ioBurst;
			if (ioQ[i].ioBurst <= 0) {
				readyQ.push_back(ioQ[i]);
				ioQ.erase(ioQ.begin()+i);
			}
		}
	}

	void checkIO(vector<PCB>& ioQ) {
		//cout << "\n\t\t\t\tDismiss IO & sizeL:" << ioQ.size() << endl;
		for (int i = 0; i < ioQ.size(); i++) {
			cout << "\n\t\t Process " << ioQ[i].number << " I/O Time: " << ioQ[i].ioBurst;
		}
	}

	int longestIO(vector<PCB>& ioQ) {
		int longest = ioQ[0].ioBurst;
		for (int i = 1; i < ioQ.size(); i++) {
			if (ioQ[i].ioBurst > longest) {
				longest = ioQ[i].ioBurst;
			}
		}
		return longest;
	}

	int getInstructionLength(const int process[], int number) {
		int i = 0, length = 0;
		cout << "\nP" << number << ": {  ";
		while (process[i]) {
			cout << process[i] << "  ";
			length++;
			i++;
		}
		cout << "}";
		return length;
	}

	void processLengths(vector<PCB>& readyQ) {
		cout << "\n\nProcess instructions lengths:";
		for (int i = 0; i < readyQ.size(); i++) {
			cout << "\n\tProcess " << i << ": " << readyQ[i].length;
		}
	}

	void showStats(vector<PCB>& processes) {
		;
	}
}

//schedulers
namespace sch {
	void FirstComeFirstServe() {
		int clock, readypos = 0, procpos = 0,
			bursttime = 0, time_elapsed = 0;
		bool ioburst;
		vector<PCB> readyQ, cpuQ, ioQ, terminateQ;

		vector<int> ioBurst;
		loadPrograms(readyQ);


		processLengths(readyQ);

		clock = 0;
		cout << "\n---------------------Start Simulation---------------------";
		while (true) {
			cout << "\nClock #: " << clock
				<< " - Ready queue size: " << readyQ.size()
				<< " - I/O queue size: " << ioQ.size();

			//determine which burst is being processed primarily
			if (readyQ.size() > 0) {
				cout << " -- CPU BURST";
				cpuQ.push_back(readyQ[0]);
				cpuQ[0].accessed = true;
				readyQ.erase(readyQ.begin());
				bursttime = cpuQ[0].instructions[cpuQ[0].counter];
				cout << "\n\tProcessing: P" << cpuQ[0].number << " instruction #" << cpuQ[0].counter << " for " << bursttime << " cycles.";
			}
			else if (ioQ.size() > 0 && readyQ.size() < 1) {
				cout << "-- IO BURST";
				bursttime = longestIO(ioQ);
				cout << "\n\tSending to IO for " << bursttime << " cycles.";
			}


			
			//start simulation
			time_elapsed = 0;
			checkIO(ioQ);
			while (bursttime > 0) {
				//add time to any waiting processes
				if (readyQ.size() > 0) {
					addTime(readyQ);
				}
				//increase simulation counters
				clock++;
				bursttime--;
				time_elapsed++;
				//process IO bursts
				decreaseIO(ioQ);
				dismissIO(ioQ, readyQ);
			}
			cout << "\n\tBurst length: " << time_elapsed;
			checkIO(ioQ);

			if (cpuQ.size()) {
				//increase process counter
				cpuQ[0].counter++;

				cout << "\nCpu counter/instructions: " << cpuQ[0].counter << "/" << cpuQ[0].length;
				
				//terminate finished process
				if (cpuQ[0].counter >= cpuQ[0].length) {
					cout << "\n\tTerminating process P" << cpuQ[0].number;
					terminateQ.push_back(cpuQ[0]);
					cpuQ.erase(cpuQ.begin());
				}
				
				//send process to IO queue after completed CPU burst
				if (cpuQ.size()) {
					ioQ.push_back(cpuQ[0]);
					ioQ.back().ioBurst = ioQ.back().instructions[ioQ.back().counter];
					cpuQ.erase(cpuQ.begin());
				}
				
			}

			cout << "\nTerminate queue size: " << terminateQ.size();
			cout << endl;


			if (terminateQ.size() == 8) {
				break;
			}
		}



	}

}