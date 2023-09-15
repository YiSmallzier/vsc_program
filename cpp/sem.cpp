#include <iostream>
using namespace std;

const int sizeofbuffer = BUFF_SIZE;
semaphore s = 1, n = 0, e = sizeofbuffer;

void producer () {
    while (true) {
        produce();
        semWait(e);
        semWait(s);
        append();
        
        semSignal(n);
        semSignal(s);
    }
}

void consumer() {
    while (true) {
        semWait(n);
        semWait(s);
        take();
        semSignal(s);
        semSignal(e);
        consume();
    }
}

void main() {
    parbegin(producer, consumer);
}

