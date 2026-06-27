// TLE 解法：用循环模拟加法，volatile 防止编译器优化
#include <bits/stdc++.h>
using namespace std;
int main(){
    long long a,b;
    cin>>a>>b;
    volatile long long ans=0;
    if(b>=0) for(long long i=0;i<b;i++) ans++;
    else for(long long i=0;i<-b;i++) ans--;
    long long real=ans;
    real+=a;
    cout<<real<<endl;
    return 0;
}
