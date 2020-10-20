#include "types/thread.h"
#include <iostream>
using namespace std;

// TODO: implement the behavior for your thread methods (optional)
int Thread::getBurstTime() {
	//if(!bursts.empty()) {
		Burst* currBurst = bursts.front();
		//bursts.pop();
		//cout << "Burst: " << currBurst->length << endl;
		return currBurst->length;
	//}
	//return -3;
}
