#include "schedulers.h"
#include <iostream>
#include <vector>

using namespace std;

//misc
namespace {
	void sayChoice(int choice) {
		int const FirstComeFirstServe = 0, ShortestJobFirst = 1, MultiLevelFeedbackQueue = 2;
		cout << "\n\tSCHEDULER: ";
		switch (choice) {
		case FirstComeFirstServe:
			cout << "First Come First Serve";
			break;
		case ShortestJobFirst:
			cout << "Shortest Job First";
			break;
		case MultiLevelFeedbackQueue:
			cout << "Multilevel Feedback Queue";
			break;
		}
	}
}

//constants
namespace sch {
	const int P1[] = { 5, 27, 3, 31, 5, 43, 4, 18, 6, 22, 4, 26, 3, 24, 4 , -1};
	const int P2[] = { 4, 48, 5, 44, 7, 42, 12, 37, 9, 76, 4, 41, 9, 31, 7, 43, 8 , -1};
	const int P3[] = { 8, 33, 12, 41, 18, 65, 14, 21, 4, 61, 15, 18, 14, 26, 5, 31, 6 , -1};
	const int P4[] = { 3, 35, 4, 41, 5, 45, 3, 51, 4, 61, 5, 54, 6, 82, 5, 77, 3 , -1};
	const int P5[] = { 16, 24, 17, 21, 5, 36, 16, 26, 7, 31, 13, 28, 11, 21, 6, 13, 3, 11, 4 , -1};
	const int P6[] = { 11, 22, 4, 8, 5, 10, 6, 12, 7, 14, 9, 18, 12, 24, 15, 30, 8 , -1};
	const int P7[] = { 14, 46, 17, 41, 11, 42, 15, 21, 4, 32, 7, 19, 16, 33, 10 , -1};
	const int P8[] = { 4, 14, 5, 33, 6, 51, 14, 73, 16, 87, 6 , -1};

	const int* PROGRAMS[] = { P1, P2, P3, P4, P5, P6, P7, P8 };

	const int NEW = 0, READY = 1, CPU = 2, IO = 3, TERMINATE = 4;

	int const FirstComeFirstServe = 0, ShortestJobFirst = 1, MultiLevelFeedbackQueue = 2;

}

//simulation
namespace sch {
	void CPU_Simulation(int schedule_choice) {
		int clock, readypos = 0, procpos = 0,
			bursttime = 0, time_elapsed = 0, total_programs, overhead = 0;
		bool ioburst;
		vector<PCB> readyQ, cpuQ, ioQ, terminateQ;

		vector<int> ioBurst;
		loadPrograms(readyQ);
		total_programs = readyQ.size();

		processLengths(readyQ);

		clock = 0;
		cout << "\n---------------------Start Simulation---------------------";
		cout << terminateQ.size();
		while (terminateQ.size() < total_programs) {
			cout << "\nClock #: " << clock
				<< " - Ready queue size: " << readyQ.size()
				<< " - I/O queue size: " << ioQ.size();

			//determine which burst is being processed primarily
			if (readyQ.size() > 0 && cpuQ.size() == 0) {
				cout << " -- CPU BURST";
				readypos = selectSchedule(readyQ, schedule_choice);
				cpuQ.push_back(readyQ[readypos]);
				cpuQ[0].accessed = true;
				readyQ.erase(readyQ.begin()+readypos);
				bursttime = cpuQ[0].instructions[cpuQ[0].counter];
				cout << "\n\tProcessing: P" << cpuQ[0].number << " instruction #" << cpuQ[0].counter << " for " << bursttime << " cycles.";
				clock++;
				overhead++;
			}
			else if (ioQ.size() > 0 && readyQ.size() < 1) {
				cout << "-- IO BURST";
				bursttime = longestIO(ioQ);
				cout << "\n\tSending to IO for " << bursttime << " cycles.";
			}

			//start simulation
			time_elapsed = 0;
			checkReady(readyQ);
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


			if (cpuQ.size()) {
				cpuQ[0].cputime += time_elapsed;

				//increase process counter
				cpuQ[0].counter++;

				cout << "\n\tCpu counter/instructions: " << cpuQ[0].counter << "/" << cpuQ[0].length;
				
				//terminate finished process
				if (cpuQ[0].counter >= cpuQ[0].length) {
					cout << "\n\tTerminating process P" << cpuQ[0].number;
					terminateQ.push_back(cpuQ[0]);
					cpuQ.erase(cpuQ.begin());
					clock++;
					overhead++;
				} 
				
				//send process to IO queue after completed CPU burst
				if (cpuQ.size()) {
					ioQ.push_back(cpuQ[0]);
					ioQ.back().ioBurst = ioQ.back().instructions[ioQ.back().counter];
					cpuQ.erase(cpuQ.begin());
					clock++;
					overhead++;
				}	
			}

			checkReady(readyQ);
			checkIO(ioQ);

			cout << "\nTerminate queue size: " << terminateQ.size();
			cout << endl;
		}

		sayChoice(schedule_choice);
		showStats(terminateQ, clock, overhead);
	}
}

//simulation functions
namespace sch {
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

	void checkReady(vector<PCB>& readyQ) {
		cout << "\n\t\tProcesses in readyQ:\n\t\t\t";
		for (int i = 0; i < readyQ.size(); i++) {
			cout << "P" << readyQ[i].number;
			if (i < readyQ.size() - 1) {
				cout << ", ";
			}
		}
	}

	void decreaseIO(vector<PCB>& ioQ) {
		for (int i = 0; i < ioQ.size(); i++) {
			ioQ[i].ioBurst--;
			ioQ[i].iotime++;
		}

	}

	void dismissIO(vector<PCB>& ioQ, vector<PCB>& readyQ) {
		//cout << "\n\t\t\t\tDismiss IO & sizeL:" << ioQ.size() << endl;
		for (int i = 0; i < ioQ.size(); i++) {
			//cout << "\n\t\t Process " << ioQ[i].number << " I/O burst time: " << ioQ[i].ioBurst;
			if (ioQ[i].ioBurst <= 0) {
				ioQ[i].counter++;
				readyQ.push_back(ioQ[i]);
				ioQ.erase(ioQ.begin() + i);
			}
		}
	}

	void checkIO(vector<PCB>& ioQ) {
		cout << "\n\t\t" << "Processes in ioQ:";
		for (int i = 0; i < ioQ.size(); i++) {
			cout << "\n\t\t\tP" << ioQ[i].number << " - " << ioQ[i].ioBurst << "/" << ioQ[i].instructions[ioQ[i].counter];
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
		while (process[i] != -1) {
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

	void showStats(vector<PCB>& procs, int clock, int overhead) {
		int turnaround;
		cout << "\n-----------------------------------------------------"
			<< "\n\tClock: " << clock << "\t|\tOverhead: "  << overhead
			<< "\n-----------------------------------------------------"
			<< "\nP#\tTr\tTw\tTcpu\tTio\tTtr";
		for (int i = 0; i < 8; i++) {
			turnaround = 0;
			cout << "\nP" << procs[i].number
				<< "\t" << procs[i].responsetime
				<< "\t" << procs[i].waittime
				<< "\t" << procs[i].cputime
				<< "\t" << procs[i].iotime
				<< "\t" << procs[i].cputime + procs[i].iotime + procs[i].waittime;
			/*for (int j = 0; j < procs[i].length; j++) {
				turnaround += procs[i].instructions[j];
			}
			cout << "\t" << turnaround;
			turnaround += procs[i].waittime;
			cout << "\t" << turnaround;*/
		}
	}

	
}

//schedulers
namespace sch {
	int selectSchedule(vector<PCB>& readyQ, int choice) {
		return selectSchedule(readyQ, choice, 0);
	}

	int selectSchedule(vector<PCB>& readyQ, int choice, int levels) {
		switch (choice) {
		case FirstComeFirstServe:
			return FCFS();
		case ShortestJobFirst:
			return SJF(readyQ);
		case MultiLevelFeedbackQueue:
			return FCFS();
		}
	}

	int FCFS() {
		return 0;
	}

	int SJF(vector<PCB>& readyQ) {
		PCB test = readyQ[0];
		int shortest = 0;
		for (int i = 0; i < readyQ.size(); i++) {
			if (readyQ[i].instructions[readyQ[i].counter] < test.instructions[test.counter]) {
				test = readyQ[i];
				shortest = i;
			}
		}
		return shortest;
	}


}
