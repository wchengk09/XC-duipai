#include <bits/stdc++.h>
#include <unistd.h>
using namespace std;
mt19937 rnd(getpid());

int main(){
    printf("1\n1 2\n");
    int a[3] = {rnd() % 5 + 1,rnd() % 5 + 1,rnd() % 5 + 1};
	sort(a,a + 3);
	printf("%d\n",a[1]);
	printf("0 %d\n1 %d\n",a[0],a[2]);
    return 0;
}