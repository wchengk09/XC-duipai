#include <bits/stdc++.h>
#include <unistd.h>
using namespace std;
const int n = 100;
mt19937 rnd(getpid());

int main(){
    printf("1\n%d\n",n);
    for (int i = 1;i <= n;i ++)
        printf("%d ",rnd() % n + 1);
    return 0;
}