#include "synch.h"

Semaphore* insertSem;
Semaphore* removeSem;
Lock* listLock;
Condition* listEmptyCond;
Condition* listFullCond;
List *dllist;

void Insert_to_DLList(List *dllist) {
    if (dllist->IsEmpty()) {
        printf("Insert_to_DLList: List Empty, inserting 1\n");
    }
    dllist->SortedInsert((void*)1, 1);
    if (dllist->IsEmpty()) {
        printf("Insert_to_DLList: After insertion, List Empty\n");
    }
}

void Remove_from_DLList(List *dllist) {
    if (!dllist->IsEmpty()) {
        printf("Remove_from_DLList: Removing last element\n");
        dllist->Remove(dllist->Last());
    } else {
        printf("Remove_from_DLList: List is Empty\n");
    }
}

void DLLInsertThread(int arg) {
    int i;
    for (i = 0; i < 5; i++) {
        printf("*** insert_thread %d looped %d times\n", arg, i);
        insertSem->P(); // acquire insert semaphore
        listLock->Acquire(); // acquire lock
        Insert_to_DLList(dllist);
        listFullCond->Signal(listLock); // signal list not full
        listLock->Release(); // release lock
        removeSem->V(); // release remove semaphore
    }
}

void DLLDeleteThread(int arg) {
    int i;
    for (i = 0; i < 5; i++) {
        printf("*** remove_thread %d looped %d times\n", arg, i);
        removeSem->P(); // acquire remove semaphore
        listLock->Acquire(); // acquire lock
        while (dllist->IsEmpty()) { // check if list is empty
            listEmptyCond->Wait(listLock); // if empty, wait for signal
        }
        Remove_from_DLList(dllist);
        listEmptyCond->Signal(listLock); // signal list not empty
        listLock->Release(); // release lock
        insertSem->V(); // release insert semaphore
    }
}

void ThreadTest1() {
    DEBUG('t', "Entering ThreadTest1");
    insertSem = new Semaphore("insertSem", 1);
    removeSem = new Semaphore("removeSem", 0);
    listLock = new Lock("listLock");
    listEmptyCond = new Condition("listEmptyCond");
    listFullCond = new Condition("listFullCond");
    dllist = new List();

    Thread *t1 = new Thread("insert thread");
    t1->Fork(DLLInsertThread, 0);
    Thread *t2 = new Thread("delete thread");
