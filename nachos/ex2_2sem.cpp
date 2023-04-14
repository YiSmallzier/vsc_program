class Lock {
public:
    Lock(char* debugName);
    ~Lock();
    void Acquire();
    void Release();
private:
    Semaphore *lock; // binary semaphore
};
Lock::Lock(char* debugName) {
    lock = new Semaphore(debugName, 1);
}

Lock::~Lock() {
    delete lock;
}

void Lock::Acquire() {
    lock->P(); // P操作，获取锁
}

void Lock::Release() {
    lock->V(); // V操作，释放锁
}


class Condition {
public:
    Condition(char* debugName, Lock* conditionLock);
    ~Condition();
    void Wait();
    void Signal();
    void Broadcast();
private:
    char* name;
    Lock* lock;
    List* queue;
};
Condition::Condition(char* debugName, Lock* conditionLock) {
    name = debugName;
    lock = conditionLock;
    queue = new List();
}

Condition::~Condition() {
    delete queue;
}

void Condition::Wait() {
    lock->Release(); // 释放锁，等待条件变量
    queue->Append((void *)currentThread);
    lock->Acquire(); // 重新获取锁
    currentThread->Sleep(); // 等待被唤醒
}

void Condition::Signal() {
    Thread* t;
    if (!queue->IsEmpty()) {
        t = (Thread *)queue->Remove();
        scheduler->ReadyToRun(t); // 唤醒等待的线程
    }
}

void Condition::Broadcast() {
    Thread* t;
    while (!queue->IsEmpty()) {
        t = (Thread *)queue->Remove();
        scheduler->ReadyToRun(t); // 唤醒等待的线程
    }
}
