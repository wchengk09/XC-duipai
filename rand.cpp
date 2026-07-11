#include <bits/stdc++.h>
using namespace std;
const int T = 1;
const int n = 5;
mt19937 rnd((random_device())());

int main(){
    printf("%d\n",T);
    for (int _ = 1;_ <= T;_ ++){
        printf("%d %d\n",n,rnd() % 10 + 1);
        for (int i = 1;i <= n;i ++)printf("%c","01"[rnd() & 1]);
        printf("\n");
    }
    return 0;
}