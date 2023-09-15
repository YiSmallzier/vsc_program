#include <iostream>
#include <queue>
using namespace std;

void semWait(s) {
    if(s.count > 0) {
        s.count--;
    }
    else {
        place this process in s.queue;
        block;
    }

}


void semSignal (s) {
    if(there is at least one process blocked on semaphore s ) {
        remove a process P from s.queue
        place process P on ready list;
    }
    else 
        s.count++;
}
