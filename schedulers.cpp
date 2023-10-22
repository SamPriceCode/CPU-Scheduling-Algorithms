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
		//variable declaration
		int clock = 0,			//clock
			CPU_unused = 0.0,		//checks how long the CPU is unused
			bursttime = 0,		//length of a given CPU or IO burst
			time_elapsed = 0,	//amount of time a burst lasts
			scheduler_pos = 0,	//position of process to retrieve from given scheduling algorithm
			total_programs = 0,	//total amount of programs being processes
			time_quantum = cycle_limits[0],	//time quantum for a given cycle, starts at Queue 1
			queue_counter = 0,	//determines which queue is supposed to be used
			ready_iteration = 0;		//how many processes have been iterated through in the readyQ, used to determine when to swap queues
		
		vector<PCB> readyQ,		//Ready Queue
					cpuQ,		//CPU, not a queue but is a vector for sake of compatibility. should only ever contain 1 item.
					ioQ,		//IO Queue
					terminateQ;	//Terminate Queue, basically moot, only exists to send to stats function afterwards

		bool CPUburst = false;

		//load programs into ready queue (they all arrive at zero as per assignment instructions)
		loadPrograms(readyQ);

		//find total number of programs (it's 8 but who wants to hard-code things?)
		total_programs = readyQ.size();

		//display current process lengths
		processLengths(readyQ);
		
		//start simulation
		//everything outside of the main simulation is assumed to happen instantaneously
		cout << "\n---------------------Start Simulation---------------------";
		while (terminateQ.size() < total_programs) {
			//display current clock tick, ready queue size, and IO queue size
			cout << "\nClock #: " << clock
				<< " - Ready Queue size: " << readyQ.size()
				<< " - I/O Queue size: " << ioQ.size();

			//determine which burst is being processed
			//If there is a process in the ready queue: CPU bound burst
			if (readyQ.size() > 0) {
				cout << " -- CPU BURST";

				//use desired scheduling algorithm to find process position in ready queue
				scheduler_pos = selectSchedule(readyQ, schedule_choice);
				//send process to CPU
				cpuQ.push_back(readyQ[scheduler_pos]);
				//mark that the process has be accessed
				cpuQ[0].accessed = true;
				//remove from ready queue
				readyQ.erase(readyQ.begin()+scheduler_pos);

				//if there is no leftover CPU burst, use current instruction
				if (cpuQ[0].cpuBurst == 0) {
					bursttime = cpuQ[0].instructions[cpuQ[0].counter];
				}
				//if there is a leftover CPU burst, finish it
				else {
					bursttime = cpuQ[0].cpuBurst;
				}
				cout << "\n\tProcessing: P" << cpuQ[0].number + 1 << " instruction #" << cpuQ[0].counter << " for " << bursttime << " cycles.";\

				//increment
				ready_iteration++;

				//declare that this is a CPU burst
				CPUburst = true;
			}
			//If there are no processes in the ready queue: IO bound burst
			else if (ioQ.size() > 0 && readyQ.size() < 1) {
				cout << " -- IO BURST";

				//determine shortest queued IO process
				bursttime = shortestIO(ioQ);
				cout << "\n\tSending to IO for " << bursttime << " cycles.";

				//declare that this is not a CPU burst
				CPUburst = false;
			}

			//only activate time quantum functions on CPU bursts
			if (CPUburst) {
				//increment desired quantum Queue and reset ready Queue position
				if (ready_iteration == total_programs) {
					queue_counter++;
					ready_iteration = 0;
				}

				//decide which time quantum Queue is appropriate
				//set to queue 2
				if (queue_counter < 2) {
					time_quantum = cycle_limits[queue_counter];
				}
				//set to queue 3
				else if (queue_counter == 2) {
					time_quantum = 999;
				}
				//reset to queue 0
				else if (queue_counter > 2) {
					queue_counter = 0;
					time_quantum = cycle_limits[queue_counter];
				}
			}
			else {
				//IO proceses are not limited by time quantum
				time_quantum = 999;
			}

			//cout << "\n\t\t\t\t---ready_iteration: " << ready_iteration << " ---T_Qantum: " << time_quantum << " ---Counter:" << queue_counter;

			//set main simulation time to 0
			time_elapsed = 0;

			//output Ready and IO Queue before execution
			checkReady(readyQ);
			checkIO(ioQ);

			//start burst execution
			while (bursttime > 0) {
				//add unused CPU time
				if (!CPUburst) {
					CPU_unused++;
				}

				//add time to any waiting processes
				if (readyQ.size() > 0) {
					addTime(readyQ);
				}

				//increment clock
				clock++;
				//decrement current burst time left
				bursttime--;
				//increment the amount of time elapsed in the current burst
				time_elapsed++;
				//decrement amount time quantum left in cycle
				time_quantum--;

				//decrease burst counter for all executing IO processes
				decreaseIO(ioQ);
				
				//dismiss any finished IO processes to the Ready Queue
				dismissIO(ioQ, readyQ);

				//end current burst if time quantum runs out
				if (time_quantum == 0) {
					cout << "\n\t\t\t\t------------------TIME QUANTUM RAN OUT";
					break;
				}
			}
	
			//output how long the burst lasted
			cout << "\n\tBurst length: " << time_elapsed;
			
			//cout << "\n\t\t\t\t---ready_iteration: " << ready_iteration << " ---T_Qantum: " << time_quantum << " ---Counter:" << queue_counter;

			//handle process currently in CPU
			if (cpuQ.size()) {
				//increase total running CPU time
				cpuQ[0].cputime += time_elapsed;

				//increase process counter if CPU burst was completed	
				if (time_quantum > 0) { cpuQ[0].counter++; }

				//output current couonter position out of total instruction length
				//cout << "\n\tCpu counter/instructions: " << cpuQ[0].counter << "/" << cpuQ[0].length;
				
				//terminate finished process
				if (cpuQ[0].counter >= cpuQ[0].length) {
					cout << "\n\tTerminating process P" << cpuQ[0].number + 1;
					//add to Terminate Queue
					terminateQ.push_back(cpuQ[0]);
					//remmove from CPU
					cpuQ.erase(cpuQ.begin());

					//overhead is not added because Termiante Queue is not a real queue
				}
				else {
					//send process to IO Queue after completed CPU burst
					if (time_quantum > 0) {
						//set current IO burst length to be decremented later
						cpuQ[0].ioBurst = cpuQ[0].instructions[cpuQ[0].counter];
						//set CPU burst to 0; it's finished!!
						cpuQ[0].cpuBurst = 0;
						//send to IO Queue
						ioQ.push_back(cpuQ[0]);
						//remove from CPU
						cpuQ.erase(cpuQ.begin());
					}
					//send process back to ready queue is CPU burst is in complete
					else {
						//update CPU burst that is left to be completed
						cpuQ[0].cpuBurst = abs(cpuQ[0].instructions[cpuQ[0].counter] - time_elapsed);
						//send ready queue IO Queue
						readyQ.push_back(cpuQ[0]);
						//remove from CPU
						cpuQ.erase(cpuQ.begin());
					}	
				}
			}

			//output Ready and IO Queue after execution
			checkReady(readyQ);
			checkIO(ioQ);

			cout << "\nTerminate Queue size: " << terminateQ.size();
			cout << endl;
		}

		//determine if MLFQ was used
		int choice = (cycle_limits[0] == 999) ? schedule_choice : 2;

		sayChoice(choice);
		showStats(terminateQ, clock, CPU_unused);
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
	
	//prints all current processes in ready Queue and the current waiting time, and current program counter position
	void checkReady(vector<PCB>& readyQ) {
		cout << "\n\t\tProcesses in readyQ:";
		for (int i = 0; i < readyQ.size(); i++) {
			cout << "\n\t\t\tP" << readyQ[i].number + 1 << " -- " << readyQ[i].counter << "/" << readyQ[i].length
			<< ", Tw = " << readyQ[i].waittime;
				
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
			cout << "\n\t\t\tP" << ioQ[i].number + 1 << " - " << ioQ[i].ioBurst << "/" << ioQ[i].instructions[ioQ[i].counter];
		}
	}

	//finds the process with the longest current remaining IO burst
	int shortestIO(vector<PCB>& ioQ) {
		int shortest = ioQ[0].ioBurst;
		for (int i = 1; i < ioQ.size(); i++) {
			if (ioQ[i].ioBurst < shortest) {
				shortest = ioQ[i].ioBurst;
			}
		}
		return shortest;
	}

	//gets length of a process's instructions
	int getInstructionLength(const int process[], int number) {
		int i = 0, length = 0;
		cout << "\nP" << number + 1 << ": {  ";
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
			cout << "\n\tProcess " << i + 1<< ": " << readyQ[i].length;
		}
	}

	//prints the waiting, response, and turnaround time for all given processes
	void showStats(vector<PCB>& procs, int clock, int CPU_unused) {
		int turnaround = 0, inits = 0, procnum = 0;
		//header information including total clock and overhead time
		cout << "\n-----------------------------------------------------"
			<< "\n\tClock: " << clock << "\t|\tCPU Unused Time: " << CPU_unused
			<< "\n-----------------------------------------------------"
			<< "\nP#\tTr\tTw\tTcpu\tTio\tTtr";
		cout << "\t\tInital Instruction Length";

		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if (procs[j].number == i) {
					procnum = j;
					break;
				}
			}

			turnaround = 0;
			inits = 0;
			// Statistic to be printed:
			//	process number, 
			//	total response time(Tr), 
			//	total waiting time(Tw), 
			//	turnaround time(Ttr)
			//	also prints total CPU (Tcpu) and IO (Tio) time for data validation purposes
			cout << "\nP" << procs[procnum].number + 1
				<< "\t" << procs[procnum].responsetime
				<< "\t" << procs[procnum].waittime
				<< "\t" << procs[procnum].cputime
				<< "\t" << procs[procnum].iotime
				<< "\t" << procs[procnum].cputime + procs[procnum].iotime + procs[procnum].waittime;
			//this section is test code used to make sure the Tcpu and Tio times are correct
			
			for (int j = 0; j < procs[procnum].length; j++) {
				inits += procs[procnum].instructions[j];
			}
			cout << "\t\t" << inits;
			
		}

		cout << "\n-----------------------------------------------------"
			<< "\n\tCPU Utilization: " << (1 - (float(CPU_unused) / float(clock))) * 100 << "%"
			<< "\n-----------------------------------------------------";
	}

	
}

//schedulers
namespace sch {
	//input determines scheduling algorithm, 1 = FCFS, 2 = SJF
	//returns an integer value corresponding to a position in the ready list
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
