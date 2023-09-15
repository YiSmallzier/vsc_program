#include <iostream>
#include <typeinfo>

using namespace std;


bool judge(int resi, int i, int resj, int j) {
    if (resi < resj)
        return true;
    else if( resi == resj && i < j) {
        return true;
    }
    return false;
}
void init_ini (bool choosing[], int number [], int n) {
    for(int i = 0; i < n; ++ i) {
        choosing[i] = false;
        number[i] = 0;
    }

}
int getmax(int  arr[] , int n) {
    if(n == 1) {
        return arr[0];
    }
    int mxn = arr[0];
    for(int i = 1; i < n; ++ i) {
        mxn = max(arr[i], mxn);
    }
    return mxn;
}
void bakery(bool choosing[], int number [], int n) {
    
    int i = 0;
    while (true)
    {

        init_ini(choosing, number, n);

        choosing[i] = true;
        number[i] = i + getmax(number, n);
        choosing[i] = false;
        for(int j = 0; j < n; ++ j) {
            while(choosing[j]) {
                ;
            }
            while((number[j] != 0) && judge(number[i], i, number[j], j) ) {
                ;
            }
            
        }
        //临界区
        number[i] = 0;
        //其余部分
    }
}
