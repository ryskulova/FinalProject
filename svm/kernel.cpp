#include "kernel.h"

#include <iostream>
#include <algorithm>

namespace svm
{
    Kernel::Kernel(
                Scheduler scheduler,
                std::vector<Memory::ram_type> executables_paths
            )
        : board(),
          processes(),
          priorities(),
          scheduler(scheduler),
          _last_issued_process_id(0),
          _last_ram_position(0),
          _cycles_passed_after_preemption(0),
          _current_process_index(0)
    {
        std::for_each(
            executables_paths.begin(),
            executables_paths.end(),
            [&](Memory::ram_type &executable) {
                CreateProcess(executable);
            }
        );

        if (scheduler == FirstComeFirstServed) {
            board.pic.isr_0 = [&]() {
                // ToDo: Process the timer interrupt for the FCFS
            };

            board.pic.isr_3 = [&]() {
                // ToDo: Process the first software interrupt for the FCFS
                // Unload the current process
            };
        } else if (scheduler == ShortestJob) {
            board.pic.isr_0 = [&]() {
                // ToDo: Process the timer interrupt for the Shortest
                //  Job scheduler
            };

            board.pic.isr_3 = [&]() {
                // ToDo: Process the first software interrupt for the Shortest
                //  Job scheduler

                // Unload the current process
            };
        } else if (scheduler == RoundRobin) {
            board.pic.isr_0 = [&]() {
                // ToDo: Process the timer interrupt for the Round Robin
                //  scheduler
				std::cout << "Kernel: States of processes: ";
				for (unsigned int i = 0; i < processes.size(); ++i) { // Looping through the Processes
					std::cout << processes[i].state << " "; // Output the current state of of a Process
				}
				std::cout << std::endl;
				_cycles_passed_after_preemption++;
				if (_cycles_passed_after_preemption > _MAX_CYCLES_BEFORE_PREEMPTION) { 
					std::cout << "Max number of Cycles is reached )))" << std::endl;
					//switch
					_cycles_passed_after_preemption = 0;
					processes[_current_process_index].registers = board.cpu.registers;
					processes[_current_process_index].state = Process::States::Ready;
					_current_process_index++;
					std::cout << "Current Process Index incremented" << std::endl;
					if (_current_process_index >= processes.size()) {
						_current_process_index = 0;
						std::cout << "Current Process Index is set to 0" << std::endl;
					}
					board.cpu.registers = processes[_current_process_index].registers;
					processes[_current_process_index].state = Process::States::Running;
					std::cout << "Process State: Running" << std::endl;
				}
            };

            board.pic.isr_3 = [&]() {
                // ToDo: Process the first software interrupt for the
                //  Round Robin scheduler
				std::cout << "Number of Processes left = " << processes.size() << std::endl;
				processes.erase(processes.begin() + _current_process_index);
				std::cout << "Erasing" << std::endl;
				if (processes.empty()) {
					board.Stop();
					std::cout << "Processes is Empty. Board has Stopped." << std::endl;
				}
				else {
					if (_current_process_index >= processes.size()) {
						_current_process_index = 0;
					}
					board.cpu.registers = processes[_current_process_index].registers;
					processes[_current_process_index].state = Process::States::Running;
					std::cout << "Status has changed to Running" << std::endl;
				}
                // Unload the current process
            };
        } else if (scheduler == Priority) {
            board.pic.isr_0 = [&]() {
                // ToDo: Process the timer interrupt for the Priority Queue
                //  scheduler
            };

            board.pic.isr_3 = [&]() {
                // ToDo: Process the first software interrupt for the Priority
                //  Queue scheduler

                // Unload the current process
            };
        }

        // ToDo

        // ---

        board.Start();
	
    }

    Kernel::~Kernel() { }

    void Kernel::CreateProcess(Memory::ram_type &executable)
    {
        std::copy(
            executable.begin(),
            executable.end(),
            board.memory.ram.begin() + _last_ram_position
        );

        Process process(
            _last_issued_process_id++,
            _last_ram_position,
            _last_ram_position + executable.size()
        );

        _last_ram_position +=
            executable.size();

        // ToDo: add the new process to an appropriate data structure
        processes.push_back(process);

        // ToDo: process the data structure

        // ---
    }
}
