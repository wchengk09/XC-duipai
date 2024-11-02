#include <bits/stdc++.h>
using namespace std;
const int MAXN = 2e5 + 5;
int T,n;
int a[MAXN];
map<int,int> mp;
vector<pair<int,int> > vec;

int main(){
    scanf("%d",&T);
    while (T --){
        mp.clear(),vec.clear();
        scanf("%d",&n);
        for (int i = 1;i <= n;i ++)scanf("%d",a + i),mp[a[i]] ++;
        for (auto it : mp)vec.push_back(it);
        for (int i = 1;i < (int)vec.size();i ++)vec[i].second += vec[i - 1].second;
        int ans = 0x3f3f3f3f;
        for (int i = 0;i < (int)vec.size();i ++){
            if (mp[vec[i].first] == 1){
                if (i == (int)vec.size() - 1)continue;
                int mx = vec[i].first + vec[i + 1].first;
                ans = min(ans,n - ((lower_bound(vec.begin(),vec.end(),make_pair(mx,(int)0xc0c0c0c0)) - 1)->second - (!i ? 0 : vec[i - 1].second)));
            }else{
                int mx = (vec[i].first << 1);
                ans = min(ans,n - ((lower_bound(vec.begin(),vec.end(),make_pair(mx,(int)0xc0c0c0c0)) - 1)->second - (!i ? 0 : vec[i - 1].second)));
                if (i == (int)vec.size() - 1)continue;
                mx = vec[i].first + vec[i + 1].first;
                ans = min(ans,n - ((lower_bound(vec.begin(),vec.end(),make_pair(mx,(int)0xc0c0c0c0)) - 1)->second - (!i ? 0 : vec[i - 1].second)) + mp[vec[i].first] - 1);
            }
        }
        printf("%d\n",ans);
    }
    return 0;
}