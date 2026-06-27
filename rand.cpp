#include <bits/stdc++.h>
using namespace std;
mt19937 rnd((random_device())());
int main(int argc,char**argv){
    int id=argc>1?atoi(argv[1]):1;
    // 用 id 作种子，保证 hack 模式下每次尝试的数据不同
    mt19937 r(id*12345+67890);
    // 范围放大到 [-2e9, 2e9]，使 int 解法容易溢出
    long long a=((long long)r()<<1)-4294967295LL;
    long long b=((long long)r()<<1)-4294967295LL;
    cout<<a<<" "<<b<<endl;
    return 0;
}
