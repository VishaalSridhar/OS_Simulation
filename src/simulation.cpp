#include "simulation.h"
#include "types/event.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

int invoked = 1;
int quantum = 0;
void Simulation::run(const string& filename) {
    read_file(filename);

    // While their are still events to process, invoke the corresponding methods
    // to handle them.
    while (!events.empty()) {
        const Event* event = events.top();
        events.pop();

        // Invoke the appropriate method on the scheduler for the given event type.
        switch (event->type) {
        case Event::THREAD_ARRIVED:
            handle_thread_arrived(event);
            break;

        case Event::THREAD_DISPATCH_COMPLETED:
            handle_thread_dispatch_completed(event);
            break;

        case Event::PROCESS_DISPATCH_COMPLETED:
            handle_process_dispatch_completed(event);
            break;

        case Event::CPU_BURST_COMPLETED:
            handle_cpu_burst_completed(event);
            break;

        case Event::IO_BURST_COMPLETED:
            handle_io_burst_completed(event);
            break;

        case Event::THREAD_COMPLETED:
            handle_thread_completed(event);
            break;

        case Event::THREAD_PREEMPTED:
            handle_thread_preempted(event);
            break;

        case Event::DISPATCHER_INVOKED:
            handle_dispatcher_invoked(event);
            break;
        }
	stats.total_time = event->time;
	cout << "Time: " << event->time << endl;
        // Free the event's memory.
        delete event;
    }
	logger.print_statistics(stats);
}


//==============================================================================
// Event-handling methods
//==============================================================================


void Simulation::handle_thread_arrived(const Event* event) {
    // TODO: handle this event properly (feel free to modify code structure, tho)
    scheduler->enqueue(event, event->thread);
    if((active_thread == nullptr) && (invoked == 1)) {
	   Event* event_dispatch = new Event(Event::DISPATCHER_INVOKED, event->thread->arrival_time, event->thread);
	   events.push(event_dispatch);
    }
    event->thread->previous_state = event->thread->NEW;
    event->thread->current_state = event->thread->READY;
    invoked = 0;
    logger.print_state_transition(event, event->thread->previous_state, event->thread->current_state);
}


void Simulation::handle_thread_dispatch_completed(const Event* event) {
    // TODO: handle this event properly (feel free to modify code structure, tho)
    prev_thread = active_thread;
    active_thread = event->thread;
    if (scheduler->should_preempt_on_arrival(event)) {
	Event* preempt_event_1 = new Event(Event::THREAD_PREEMPTED, event->time + quantum, event->thread);
        events.push(preempt_event_1);
    }
    else {
    Event* cpu_event = new Event(Event::CPU_BURST_COMPLETED, event->time + event->thread->getBurstTime(), event->thread);
    events.push(cpu_event);
    }
  //  cout << "Burst Thread Dispatch: " << event->thread->bursts.front()->length << endl;
  //  cout << "Hi" << endl;
    event->thread->previous_state = event->thread->READY;
    event->thread->current_state = event->thread->RUNNING;
    logger.print_state_transition(event, event->thread->previous_state, event->thread->current_state);
}


void Simulation::handle_process_dispatch_completed(const Event* event) {
    // TODO: handle this event properly (feel free to modify code structure, tho)
    prev_thread = active_thread;
    active_thread = event->thread;
    //cout << "Quantum is: " << quantum << endl;
    if (scheduler->should_preempt_on_arrival(event)) {
        //cout << "1" << endl;
	Event* preempt_event = new Event(Event::THREAD_PREEMPTED, event->time + quantum, event->thread);
	events.push(preempt_event);
    }
    else {
    	Event* cpu_event_1 = new Event(Event::CPU_BURST_COMPLETED, event->time + event->thread->getBurstTime(), event->thread);
    	events.push(cpu_event_1);
    }
  //  cout << "Burst Process Dispatch: " << event->thread->bursts.front()->length << endl;
    //cout << "Hi" << endl;
    event->thread->previous_state = event->thread->READY;
    event->thread->current_state = event->thread->RUNNING;
    logger.print_state_transition(event, event->thread->previous_state, event->thread->current_state);
}


void Simulation::handle_cpu_burst_completed(const Event* event) {
    // TODO: handle this event properly (feel free to modify code structure, tho)
	stats.service_time = stats.service_time + event->thread->bursts.front()->length;    
	event->thread->bursts.pop();
    //cout << "1" << endl;
  if(scheduler->size() != 0) {
   Event* test_dispatch = new Event(Event::DISPATCHER_INVOKED, event->time, event->thread);
   events.push(test_dispatch);
 }
    if (event->thread->bursts.size() != 0) {
	  invoked = 1;
	  Event* io_event = new Event(Event::IO_BURST_COMPLETED, event->time + event->thread->getBurstTime(), event->thread);
    events.push(io_event);
    //cout << "Burst CPU BURST COMPLETED: " << event->thread->bursts.front()->length << endl;
    //cout << "Hi" << endl;
    
    }
    else {

	 Event* thread_completed = new Event(Event::THREAD_COMPLETED, event->time, event->thread);
	 events.push(thread_completed);

    }

    prev_thread = active_thread;
    //active_thread = nullptr;
    event->thread->previous_state = event->thread->RUNNING;
    event->thread->current_state = event->thread->BLOCKED;
    logger.print_state_transition(event, event->thread->previous_state, event->thread->current_state);
}


void Simulation::handle_io_burst_completed(const Event* event) {
    // TODO: handle this event properly (feel free to modify code structure, tho)
	stats.io_time = stats.io_time + event->thread->bursts.front()->length;      
	event->thread->bursts.pop();
	
      scheduler->enqueue(event, event->thread);
    //  cout << "2" << endl;
      bool isSizeOne = false;
      if (scheduler->size() == 1) {
        isSizeOne = true;
      }
      if(active_thread == nullptr || isSizeOne) {
        Event* test_dispatch = new Event(Event::DISPATCHER_INVOKED, event->time, event->thread);
        events.push(test_dispatch);
      }



    event->thread->previous_state = event->thread->BLOCKED;
    event->thread->current_state = event->thread->READY;
    logger.print_state_transition(event, event->thread->previous_state, event->thread->current_state);
}


void Simulation::handle_thread_completed(const Event* event) {
    // TODO: handle this event properly (feel free to modify code structure, tho)
    //cout << "3" << endl;
    //     Event* event_dispatcher = new Event(Event::DISPATCHER_INVOKED, event->time, event->thread);
    //     events.push(event_dispatcher);
    // invoked = 1;
    event->thread->previous_state = event->thread->RUNNING;
    event->thread->current_state = event->thread->EXIT;
    logger.print_state_transition(event, event->thread->previous_state, event->thread->current_state);
}


void Simulation::handle_thread_preempted(const Event* event) {
    // TODO: handle this event properly (feel free to modify code structure, tho)
    event->thread->bursts.front()->length = event->thread->bursts.front()->length - quantum;
    Event* disp_invok = new Event(Event::DISPATCHER_INVOKED, event->time, event->thread);
    events.push(disp_invok);
    scheduler->enqueue(event, event->thread);
    event->thread->previous_state = event->thread->RUNNING;
    event->thread->current_state = event->thread->READY;
    logger.print_state_transition(event, event->thread->previous_state, event->thread->current_state);
}


void Simulation::handle_dispatcher_invoked(const Event* event) {
    // TODO: handle this event properly (feel free to modify code structure, tho)
    	invoked = 1;
	//prev_thread = event->thread;
	SchedulingDecision* act_thread = new SchedulingDecision();
	act_thread = scheduler->get_next_thread(event);
        quantum = act_thread->time_slice;
//cout << "Ready queue size: " << scheduler->size () << endl;
	active_thread = act_thread->thread;
        //prev_thread = event->thread;
	  if(active_thread != nullptr && act_thread->thread != nullptr) {
  		if((prev_thread != nullptr) && (prev_thread->process->pid == act_thread->thread->process->pid)) {
          //active_thread = act_thread->thread;
  	      Event* event_thread_dispatched = new Event(Event::THREAD_DISPATCH_COMPLETED, event->time + thread_switch_overhead, active_thread);
  		    events.push(event_thread_dispatched);
  		    event->thread->previous_state = event->thread->READY;
      		event->thread->current_state = event->thread->RUNNING;
		stats.dispatch_time = stats.dispatch_time + thread_switch_overhead;
  		}
  		else  {
		    //active_thread = act_thread->thread;
  		    Event* event_process_dispatched = new Event(Event::PROCESS_DISPATCH_COMPLETED, event->time + process_switch_overhead, active_thread);
  		    events.push(event_process_dispatched);
  		    event->thread->previous_state = event->thread->READY;
      	  	    event->thread->current_state = event->thread->RUNNING;
		    stats.dispatch_time = stats.dispatch_time + process_switch_overhead;
  		}
	  }
    string message = "Selected from " + to_string(scheduler->size() + 1) + " threads; will run to completion of burst";
    logger.print_verbose(event, active_thread, message);
}


//==============================================================================
// Utility methods
//==============================================================================


void Simulation::add_event(Event* event) {
    if (event != nullptr) {
        events.push(event);
    }
}


void Simulation::read_file(const string& filename) {
    ifstream file(filename.c_str());

    if (!file) {
        cerr << "Unable to open simulation file: " << filename << endl;
        exit(EXIT_FAILURE);
    }

    size_t num_processes;

    // Read the total number of processes, as well as the dispatch overheads.
    file >> num_processes >> thread_switch_overhead >> process_switch_overhead;

    // Read in each process.
    for (size_t p = 0; p < num_processes; p++) {
        Process* process = read_process(file);

        processes[process->pid] = process;
    }
}


Process* Simulation::read_process(istream& in) {
    int pid, type;
    size_t num_threads;

    // Read in the process ID, its type, and the number of threads.
    in >> pid >> type >> num_threads;

    // Create the process and register its existence in the processes map.
    Process* process = new Process(pid, (Process::Type) type);

    // Read in each thread in the process.
    for (size_t tid = 0; tid < num_threads; tid++) {
        process->threads.push_back(read_thread(in, tid, process));
    }

    return process;
}


Thread* Simulation::read_thread(istream& in, int tid, Process* process) {
    int arrival_time;
    size_t num_cpu_bursts;

    // Read in the thread's arrival time and its number of CPU bursts.
    in >> arrival_time >> num_cpu_bursts;

    Thread* thread = new Thread(arrival_time, tid, process);

    // Read in each burst in the thread.
    for (size_t n = 0, burst_length; n < num_cpu_bursts * 2 - 1; n++) {
        in >> burst_length;

        Burst::Type burst_type = (n % 2 == 0)
                ? Burst::CPU
                : Burst::IO;

        thread->bursts.push(new Burst(burst_type, burst_length));
    }

    // Add an arrival event for the thread.
    events.push(new Event(Event::THREAD_ARRIVED, thread->arrival_time, thread));

    return thread;
}


SystemStats Simulation::calculate_statistics() {
    // TODO: your code here (optional; feel free to modify code structure)
    return SystemStats();
}
