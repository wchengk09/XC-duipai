#include <bits/stdc++.h>
#include "testlib.h"
using namespace std;

int main(int argc,char **argv){
    registerTestlibCmd(argc,argv);
    string str = ouf.readString("01");
    for (auto it : str)
        if (it != '0' && it != '1')quitf(_wa,"WA %c",it);
    quitf(_ok,"AC");
    return 0;
}