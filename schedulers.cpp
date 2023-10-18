#include "schedulers.h"
#include <iostream>
#include <vector>

using namespace std;

//general instructions
namespace sch {
	void loadPrograms(vector<PCB>& readyQ) {
		for (int i = 0; i < 8; i++) {
			PCB process;
			process.state = READY;
			process.number = i;
			process.instructions = PROGRAMS[i];
			readyQ.push_back(process);
		}

		cout << "\nPrograms Loaded Sucessfully";
	}

	void addWait(vector<PCB>& readyQ) {
		for (int i = 0; i < readyQ.size(); i++) {
			readyQ[i].waittime++;
		}
	}

	void addResponse(vector<PCB>& readyQ) {
		for (int i = 0; i < readyQ.size(); i++) {
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
			cout << "\n\t\t Process " << i << " I/O burst time: " << ioQ[i].ioBurst;
			if (ioQ[i].ioBurst < 1) {
				readyQ.push_back(ioQ[i]);
				ioQ.erase(ioQ.begin());
			}
		}
	}
}

//First Come First Serve
namespace sch {
	void FirstComeFirstServe() {
		int clock, readypos = 0, procpos = 0,
			runtime = 0;
		bool ioburst;
		vector<PCB> readyQ, ioQ, terminateQ;

		vector<int> ioBurst;
		PCB cpu;
		loadPrograms(readyQ);

		clock = 0;
		cout << "\nStart Simulation.";
		while (true) {
			cout << "\nClock #: " << clock
				<< " - ready queue size: " << readyQ.size()
				<< " - I/O queue size: " << ioQ.size();
			if (readyQ.size() > 0) {
				clock++;
				addWait(readyQ);
				addResponse(readyQ);

				cpu = readyQ[0];
				cpu.accessed = true;
				readyQ.erase(readyQ.begin());
			}

			runtime = 0;

			cout << "\n\tProcessing Program " << cpu.number;

			while (runtime < cpu.instructions[cpu.counter]) {
				addWait(readyQ);
				addResponse(readyQ);
				clock++;
				runtime++;
				decreaseIO(ioQ);
			}
			cout << "\n\tCycle length: " << runtime;

			cpu.counter++;

			if (cpu.counter > sizeof(cpu.instructions)) {
			}

			ioQ.push_back(cpu);
			ioQ.back().ioBurst = ioQ.back().instructions[ioQ.back().counter];


			dismissIO(ioQ, readyQ);
			cout << endl;

			if (readyQ.size() < 1) {
				break;
			}
		}



	}

}