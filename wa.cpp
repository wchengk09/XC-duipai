#include <bits/stdc++.h>
using namespace std;
const int MAXN = 3e3 + 5;
int T,n,q;
int a[MAXN];

int main(){
    #ifndef kevin
    freopen("god.in","r",stdin);
    freopen("god.out","w",stdout);
    #endif

    scanf("%d",&T);
    while (T --){
        scanf("%d%d",&n,&q);
        for (int i = 1;i <= n;i ++)scanf("%d",a + i);
        for (int i = n;i >= 2;i --)a[i] -= a[i - 1];
        int x1 = 0,x2 = 0;
        for (int i = 1;i <= n;i ++){
            if (i & 1)x1 ^= a[i];
            else x2 ^= a[i];
        }
        int head = a[1],tail = a[n];
        printf("%d\n",!x1);
        while (q --){
            int op,x;
            scanf("%d%d",&op,&x);
            n ++;
            if (op == 0){
                x2 ^= x;
                x1 ^= head;
                x1 ^= head - x;
                swap(x1,x2);
                head = x;
            }
            if (op == 1){
                if (n & 1)x1 ^= x - tail;
                else x2 ^= x - tail;
                tail = x;
            }
            // printf("x1 %d x2 %d\n",x1,x2);
            printf("%d\n",!x1);
        }
    }
    return 0;
}