#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <fstream>

using namespace std;

// fstream out("out.txt", ios::out|ios::binary);

#define READ 0
#define WRITE 1
#define INS 2



const int N = 1e3 + 10;

int a[N];
int res[N];

int main()
{
	for(int i = 0; i < N; ++i) {
        a[i] = rand() % 14;
    }

    for(int i = 0; i < N; ++i) {
        // Read
        // fecth(a[i + 1]);
        // Write 
        res[i] = a[i] + 1;
        printf("%d %x\n",WRITE, &res[i]);
        // if(out_is_open())
    }

}