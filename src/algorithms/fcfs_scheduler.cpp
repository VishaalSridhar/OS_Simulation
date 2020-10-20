#include "fcfs_scheduler.h"
#include <queue>
using namespace std;


SchedulingDecision* FcfsScheduler::get_next_thread(const Event* event) {
        if(!ready_queue.empty()) {
        SchedulingDecision* temp = new SchedulingDecision();
        temp->thread = ready_queue.front();
        temp->explanation = "Explanation";
        ready_queue.pop();
        return temp;
	}
	else {
	return nullptr;
	}

}


void FcfsScheduler::enqueue(const Event* event, Thread* thread) {
    // TODO: implement me
    ready_queue.push(thread);
}


bool FcfsScheduler::should_preempt_on_arrival(const Event* event) const {
    // TODO: implement me
    return false;
}


size_t FcfsScheduler::size() const {
    // TODO: implement me
    return ready_queue.size();
}

