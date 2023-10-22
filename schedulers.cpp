#include "schedulers.h"
#include <iostream>
#include <vector>

using namespace std;

//misc functions
namespace {
	int const FirstComeFirstServe = 0, ShortestJobFirst = 1, MultiLevelFeedbackQueue = 2;

	//input a 0, 1, or 2 for FCFS, SJF, or MLFQ respectively
	void sayChoice(int choice) {
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
	const int SENTINEL = -1;

	//constant declarations for each programs to be simulated
	//sentinel values were added to the constants to make them easier to manage in execution
	const int P1[] = { 5, 27, 3, 31, 5, 43, 4, 18, 6, 22, 4, 26, 3, 24, 4 , SENTINEL};
	const int P2[] = { 4, 48, 5, 44, 7, 42, 12, 37, 9, 76, 4, 41, 9, 31, 7, 43, 8 , SENTINEL};
	const int P3[] = { 8, 33, 12, 41, 18, 65, 14, 21, 4, 61, 15, 18, 14, 26, 5, 31, 6 , SENTINEL};
	const int P4[] = { 3, 35, 4, 41, 5, 45, 3, 51, 4, 61, 5, 54, 6, 82, 5, 77, 3 , SENTINEL};
	const int P5[] = { 16, 24, 17, 21, 5, 36, 16, 26, 7, 31, 13, 28, 11, 21, 6, 13, 3, 11, 4 , SENTINEL};
	const int P6[] = { 11, 22, 4, 8, 5, 10, 6, 12, 7, 14, 9, 18, 12, 24, 15, 30, 8 , SENTINEL};
	const int P7[] = { 14, 46, 17, 41, 11, 42, 15, 21, 4, 32, 7, 19, 16, 33, 10 , SENTINEL};
	const int P8[] = { 4, 14, 5, 33, 6, 51, 14, 73, 16, 87, 6 , SENTINEL};

	//programs are stored in mother array for easy indexing through simulation
	const int* PROGRAMS[] = { P1, P2, P3, P4, P5, P6, P7, P8 };
}

//simulation
namespace sch {
	void CPU_Sim(int schedule_choice) {
		int cycle_limits[2] = {999, 999};

		CPU_Sim_Q(schedule_choice, cycle_limits);
	}

	void CPU_Sim_Q(int schedule_choice, int cycle_limits[2]) {
		int clock, schedulepos = 0, readypos = 0,
			bursttime = 0, time_elapsed = 0, overhead = 0,
			total_programs = 0,
			time_quantum = cycle_limits[0], quantum_counter = 0;
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
				<< " - Ready Queue size: " << readyQ.size()
				<< " - I/O Queue size: " << ioQ.size();

			//determine which burst is being processed primarily
			if (readyQ.size() > 0 && cpuQ.size() == 0) {
				cout << " -- CPU BURST";
				schedulepos = selectSchedule(readyQ, schedule_choice);
				cpuQ.push_back(readyQ[schedulepos]);
				cpuQ[0].accessed = true;
				readyQ.erase(readyQ.begin()+schedulepos);
				if (cpuQ[0].cpuBurst == 0) {
					bursttime = cpuQ[0].instructions[cpuQ[0].counter];
				}
				else {
					bursttime = cpuQ[0].cpuBurst;
				}
				cout << "\n\tProcessing: P" << cpuQ[0].number << " instruction #" << cpuQ[0].counter << " for " << bursttime << " cycles.";
				clock++;
				overhead++;
				readypos++;
			}
			else if (ioQ.size() > 0 && readyQ.size() < 1) {
				cout << "-- IO BURST";
				bursttime = longestIO(ioQ);
				cout << "\n\tSending to IO for " << bursttime << " cycles.";
			}

			//increment desired quantum Queue and reset ready Queue position
			if (readypos == total_programs) {
				quantum_counter++;
				readypos = 0;
			}

			//decide which time quantum Queue is appropriate
			if (quantum_counter < 2) {
				time_quantum = cycle_limits[quantum_counter];
			}
			else if (quantum_counter == 2) {
				time_quantum = 999;
			}
			else if (quantum_counter > 2) {
				quantum_counter = 0;
				time_quantum = cycle_limits[quantum_counter];
			}

			cout << "\n\t\t--Readypos: " << readypos << " ---T_Qantum: " << time_quantum << " ---Counter:" << quantum_counter;

			//start simulation
			time_elapsed = 0;
			checkReady(readyQ);
			checkIO(ioQ);
			while (bursttime > 0) {
				//add time to any waiting processes
				if (readyQ.size() > 0) {
					addTime(readyQ);
				}

				//increment simulation counters
				clock++;
				bursttime--;
				time_elapsed++;
				time_quantum--;

				//process IO bursts	
				decreaseIO(ioQ);
				dismissIO(ioQ, readyQ);

				if (time_quantum == 0) {
					cout << "\n--------------------------------------------------------------QUANTUM RAN OUT";
					break;
				}
			}
			cout << "\n\tBurst length: " << time_elapsed;
			cout << "\n\t\t---Readypos: " << readypos << " ---T_Qantum: " << time_quantum << " ---Counter:" << quantum_counter;

			if (cpuQ.size()) {
				cpuQ[0].cputime += time_elapsed;

				//increase process counter
				
				if (time_quantum > 0) { cpuQ[0].counter++; }

				cout << "\n\tCpu counter/instructions: " << cpuQ[0].counter << "/" << cpuQ[0].length;
				
				//terminate finished process
				if (cpuQ[0].counter >= cpuQ[0].length) {
					cout << "\n\tTerminating process P" << cpuQ[0].number;
					terminateQ.push_back(cpuQ[0]);
					cpuQ.erase(cpuQ.begin());
					clock++;
					overhead++;
				}
				else {
					//send process to IO Queue after completed CPU burst
					if (time_quantum > 0) {
						cpuQ[0].ioBurst = cpuQ[0].instructions[cpuQ[0].counter];
						cpuQ[0].cpuBurst = 0;
						ioQ.push_back(cpuQ[0]);
						cpuQ.erase(cpuQ.begin());
						clock++;
						overhead++;
					}
					else {
						cpuQ[0].cpuBurst = abs(cpuQ[0].instructions[cpuQ[0].counter] - time_elapsed);
						readyQ.push_back(cpuQ[0]);
						cpuQ.erase(cpuQ.begin());
						clock++;
						overhead++;
					}	
				}
			}

			checkReady(readyQ);
			checkIO(ioQ);

			cout << "\nTerminate Queue size: " << terminateQ.size();
			cout << endl;
		}

		//cout << "\ncycle limits = " << cycle_limits[0];
		int choice = (cycle_limits[0] == 999) ? schedule_choice : 2;
		sayChoice(choice);
		showStats(terminateQ, clock, overhead);
	}

}

//simulation functions
namespace sch {
	//load program P# arrays from array PROGRAMS
	void loadPrograms(vector<PCB>& readyQ) {
		//create a PCB for every loaded processes
		for (int i = 0; i < 8; i++) {
			PCB process;
			//assign process number
			process.number = i;
			//assign pointer to instructions
			process.instructions = PROGRAMS[i];
			//get the length of the process
			process.length = getInstructionLength(PROGRAMS[i], process.number);
			//add PCB to ready Queue
			readyQ.push_back(process);
		}

		cout << "\nPrograms Loaded Sucessfully";
	}

	//add wait and response time to processes in Ready Queue
	void addTime(vector<PCB>& readyQ) {
		for (int i = 0; i < readyQ.size(); i++) {
			//add wait time for any process in Ready Queue during execution
			readyQ[i].waittime++;
			//add response time for any unaccessed programs
			if (readyQ[i].accessed == false) {
				readyQ[i].responsetime++;
			}
		}
	}
	
	//prints all current processes in ready Queue
	void checkReady(vector<PCB>& readyQ) {
		cout << "\n\t\tProcesses in readyQ:\n\t\t\t";
		for (int i = 0; i < readyQ.size(); i++) {
			cout << "P" << readyQ[i].number;
			if (i < readyQ.size() - 1) {
				cout << ", ";
			}
		}
	}

	//decrease given ioBurst time for all processes in IO Queue
	void decreaseIO(vector<PCB>& ioQ) {
		for (int i = 0; i < ioQ.size(); i++) {
			//decreases IO burst time
			ioQ[i].ioBurst--;
			//increase total IO execution time
			ioQ[i].iotime++;
		}

	}

	//input IO Queue and Ready Queue, returns all ended IO processes to Ready Queue
	void dismissIO(vector<PCB>& ioQ, vector<PCB>& readyQ) {
		for (int i = 0; i < ioQ.size(); i++) {
			//if IO burst time is 0 or less, return to Ready Queue
			if (ioQ[i].ioBurst <= 0) {
				//increment program counter to next instruction
				ioQ[i].counter++;
				//put process in Ready Queue
				readyQ.push_back(ioQ[i]);
				//remove process from IO Queue
				ioQ.erase(ioQ.begin() + i);
			}
		}
	}

	//prints remaining IO burst time for all processes in IO Queue
	void checkIO(vector<PCB>& ioQ) {
		cout << "\n\t\t" << "Processes in ioQ:";
		for (int i = 0; i < ioQ.size(); i++) {
			cout << "\n\t\t\tP" << ioQ[i].number << " - " << ioQ[i].ioBurst << "/" << ioQ[i].instructions[ioQ[i].counter];
		}
	}

	//finds the process with the longest current remaining IO burst
	int longestIO(vector<PCB>& ioQ) {
		int longest = ioQ[0].ioBurst;
		for (int i = 1; i < ioQ.size(); i++) {
			if (ioQ[i].ioBurst > longest) {
				longest = ioQ[i].ioBurst;
			}
		}
		return longest;
	}

	//gets length of a process's instructions
	int getInstructionLength(const int process[], int number) {
		int i = 0, length = 0;
		cout << "\nP" << number << ": {  ";
		//keep iterating through process until sentinel value is hit
		while (process[i] != SENTINEL) {
			cout << process[i] << "  ";
			//increment instruction length amouont
			length++;
			i++;
		}
		cout << "}";
		return length;
	}

	//prints struct variable length from a given PCB
	void processLengths(vector<PCB>& readyQ) {
		cout << "\n\nProcess instructions lengths:";
		for (int i = 0; i < readyQ.size(); i++) {
			cout << "\n\tProcess " << i << ": " << readyQ[i].length;
		}
	}

	//prints the waiting, response, and turnaround time for all given processes
	void showStats(vector<PCB>& procs, int clock, int overhead) {
		int turnaround, inits = 0;
		//header information including total clock and overhead time
		cout << "\n-----------------------------------------------------"
			<< "\n\tClock: " << clock << "\t|\tOverhead: "  << overhead
			<< "\n-----------------------------------------------------"
			<< "\nP#\tTr\tTw\tTcpu\tTio\tTtr";
		//cout << "\t\tInital Instruction Length"
		for (int i = 0; i < 8; i++) {
			turnaround = 0;
			inits = 0;
			// Statistic to be printed:
			//	process number, 
			//	total response time(Tr), 
			//	total waiting time(Tw), 
			//	turnaround time(Ttr)
			//	also prints total CPU (Tcpu) and IO (Tio) time for data validation purposes
			cout << "\nP" << procs[i].number + 1
				<< "\t" << procs[i].responsetime
				<< "\t" << procs[i].waittime
				<< "\t" << procs[i].cputime
				<< "\t" << procs[i].iotime
				<< "\t" << procs[i].cputime + procs[i].iotime + procs[i].waittime;
			//this section is test code used to make sure the Tcpu and Tio times are correct
			/*
			for (int j = 0; j < procs[i].length; j++) {
				inits += procs[i].instructions[j];	
			}
			cout << "\t\t" << inits;
			*/
		}
		//print total CPU utilization
		cout << "\n-----------------------------------------------------"
			<< "\n\tCPU Utilization: " << (1 - float(overhead) / float(clock)) * 100 << "%"
			<< "\n-----------------------------------------------------";
	}

	
}

//schedulers
namespace sch {
	//determines scheduling algorithm, 1 = FCFS, 2 = SJF
	int selectSchedule(vector<PCB>& readyQ, int choice) {
		switch (choice) {
		case FirstComeFirstServe:
			return FCFS();
		case ShortestJobFirst:
			return SJF(readyQ);
		}
	}

	//First Come First Serve Scheduling Algorithm
	// - return first position the ready queue
	int FCFS() {
		//yes really that's it, it just loads the top of the ready queue
		//honestly kinda funny
		//this is also the tertiary scheduling algorithm for MLFQ
		return 0;
	}

	//Shortest Job First Scheduling Algorithm
	// - finds the process with the lowest burst length
	// - returns it's position the ready queue
	int SJF(vector<PCB>& readyQ) {
		//sets first Ready queue item as the test
		PCB test = readyQ[0];
		//initalize shortest length as zero
		int shortest_length = 0;
		//iterate through entire ready queue
		for (int i = 1; i < readyQ.size(); i++) {
			//test to see if any following instruction has a shorter burst time
			//if yes, make it the new shortest length
			if (readyQ[i].instructions[readyQ[i].counter] < test.instructions[test.counter]) {
				test = readyQ[i];
				shortest_length = i;
			}
		}
		return shortest_length;
	}


}
