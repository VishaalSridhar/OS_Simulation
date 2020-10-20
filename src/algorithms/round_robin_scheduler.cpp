#include "round_robin_scheduler.h"
#include <iostream>
using namespace std;


SchedulingDecision* RoundRobinScheduler::get_next_thread(const Event* event) {
    // TODO: implement me
     if(!ready_queue_rr.empty()) {
        SchedulingDecision* temp = new SchedulingDecision();
        temp->thread = ready_queue_rr.front();
        temp->explanation = "Explanation";
	temp->time_slice = time_slice;
        ready_queue_rr.pop();
        return temp;
	}
	else {
	return nullptr;
	}
}


void RoundRobinScheduler::enqueue(const Event* event, Thread* thread) {
    // TODO: implement me
	ready_queue_rr.push(thread);
}


bool RoundRobinScheduler::should_preempt_on_arrival(const Event* event) const {
    // TODO: implement me
    if (event->thread->bursts.front()->length > time_slice) {
	return true;
    }
    else {
	//cout << "It's false" << endl;
	//system("pause");
	return false;
    }
}


size_t RoundRobinScheduler::size() const {
    // TODO: implement me
    return ready_queue_rr.size();
}
