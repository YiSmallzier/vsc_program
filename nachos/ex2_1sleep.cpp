/**
 * @file ex2_1.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-11
 * 
 * @copyright Copyright (c) 2023
 * 
 * 注意，在实现条件变量的 Wait 方法时，需要在释放锁之后立即睡眠当前线程，这样其他线程就有机会去抢占 CPU。
在线程被唤醒后，Wait 方法会再次获得锁。此外，在实现条件变量的 Signal 和 Broadcast 方法时，需要注意将等待线程从条件变量的等待队列中移除，
并将其添加到线程调度器的就绪队列中。
 */

#ifndef SYNCH_H
#define SYNCH_H

#include "copyright.h"
#include "thread.h"
#include "list.h"

class Lock {
  public:
    Lock(char* debugName);               // initialize lock to be FREE
    ~Lock();                             // deallocate lock

    char* getName() { return name; }     // debugging assist

    void Acquire();                      // these are the only operations on a lock
    void Release();                      // release the lock, waking up a thread
                                          // waiting on the lock if possible

  private:
    char* name;                          // debugging assist
    Thread* heldBy;                      // NULL if lock is free, otherwise
                                          // a pointer to the thread that
                                          // acquired it.
   										 // plus some other stuff you'll need to define
    Condition* waitQueue;                // the condition variable used to
                                          // implement the sleep/wakeup mechanism
};

Lock::Lock(char* debugName) {
    name = debugName;
    heldBy = NULL;
    waitQueue = new Condition("lockQueue");
}

void Lock::Acquire() {
    ASSERT(!isHeldByCurrentThread());
    waitQueue->Wait(this);
    heldBy = currentThread;
}

void Lock::Release() {
    ASSERT(isHeldByCurrentThread());
    heldBy = NULL;
    waitQueue->Signal(this);
}

class Condition {
  public:
    Condition(char* debugName);          // initialize condition to "no one waiting"
    ~Condition();                        // deallocate the condition

    char* getName() { return name; }     // debugging assist

    void Wait(Lock *conditionLock);      // these are the 3 operations on condition variables;
                                         // releasing the lock and going to sleep are *atomic* in Wait()
    void Signal(Lock *conditionLock);    // conditionLock must be held by the currentThread for all of these operations
    void Broadcast(Lock *conditionLock);

  private:
    char* name;                          // debugging assist
    // plus some other stuff you'll need to define
    List* waiters;                       // list of waiting threads
};

Condition::Condition(char* debugName) {
    name = debugName;
    waiters = new List;
}

Condition::~Condition() {
    delete waiters;
}

void Condition::Wait(Lock* conditionLock) {
    ASSERT(conditionLock->isHeldByCurrentThread());
    waiters->Append((void*)currentThread);
    conditionLock->Release();
    currentThread->Sleep();
    conditionLock->Acquire();
}

void Condition::Signal(Lock* conditionLock) {
    ASSERT(conditionLock->isHeldByCurrentThread());
    if (!waiters->IsEmpty()) {
        Thread* t = (Thread*)waiters->Remove();
        scheduler->ReadyToRun(t);
    }
}

void Condition::Broadcast(Lock* conditionLock) {
    ASSERT(conditionLock->isHeldByCurrentThread());
    while (!waiters->IsEmpty()) {
        Thread* t = (Thread*)waiters->Remove();
        scheduler->ReadyToRun(t);
    }
}


