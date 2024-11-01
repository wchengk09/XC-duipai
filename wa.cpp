#include <bits/stdc++.h>
using namespace std;
const int MAXN = 2e5 + 5;
int T,n;
int a[MAXN];
map<int,vector<int>,greater<int> > mp;

namespace DSU{
    int fa[MAXN],sz[MAXN];
    int status[MAXN];
    bool active[MAXN];
    int stat[MAXN];
    int sum,okcnt;

    void init(int n){
        for (int i = 1;i <= n;i ++)
            fa[i] = i,sz[i] = 1,active[i] = false,stat[i] = 0;
        sum = okcnt = 0;
    }

    inline int f(int x){return (x + 1) >> 1;}
    inline int g(int sz,int st){return st && (!(sz & 1) || st == 1 || st == 3);}

    inline void act(int x,bool ismax){
        // printf("activate %d %d ",x,ismax);
        active[x] = true;
        sum ++;
        if (ismax){
            okcnt ++;
            stat[x] = 1;
        }
        // printf("okcnt %d\n",okcnt);
    }

    int find(int x){
        if (x == fa[x])return x;
        return fa[x] = find(fa[x]);
    }

    inline void merge(int x,int y){
        // printf("merge %d %d\n",x,y);
        assert(x < y);
        if (!active[x] || !active[y])return;
        x = find(x),y = find(y);
        int szx = sz[x],stx = stat[x],sty = stat[y];
        if (x == y)return;
        if (sz[x] < sz[y])swap(x,y);
        fa[y] = x;
        sum -= f(sz[x]);
        sum -= f(sz[y]);
        okcnt -= g(sz[x],stat[x]);
        okcnt -= g(sz[y],stat[y]);
        sz[x] += sz[y];
        sum += f(sz[x]);
        if (stx == 3 || sty == 3)stat[x] = 3;
        else if (stx && sty){
            if (szx & 1)sty = 3 - sty;
            if (stx == sty)stat[x] = sty;
            else stat[x] = 3;
        }else if (stx && !sty)stat[x] = stx;
        else if (!stx && sty){
            if (szx & 1)stat[x] = 3 - sty;
            else stat[x] = sty;
        }else stat[x] = 0;
        okcnt += g(sz[x],stat[x]);
        // printf("sz %d stat %d okcnt %d\n",sz[x],stat[x],okcnt);
    }
}

int main(){
    scanf("%d",&T);
    while (T --){
        scanf("%d",&n);
        DSU::init(n);
        mp.clear();
        int maxval = 0xc0c0c0c0;
        for (int i = 1;i <= n;i ++){
            scanf("%d",a + i);
            maxval = max(maxval,a[i]);
            mp[a[i]].push_back(i);
        }
        int ans = 0;
        for (const auto &val : mp){
            for (auto idx : val.second){
                DSU::act(idx,val.first == maxval);
                if (idx != 1)DSU::merge(idx - 1,idx);
                if (idx != n)DSU::merge(idx,idx + 1);
            }
            // printf("val %d sum %d okcnt %d\n",val.first,DSU::sum,DSU::okcnt);
            ans = max(ans,val.first + DSU::sum - (!DSU::okcnt ? 1 : 0));
        }
        printf("%d\n",ans + maxval);
    }
    return 0;
}