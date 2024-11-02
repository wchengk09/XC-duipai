#include <bits/stdc++.h>
using namespace std;
vector<int> v;
int T,n;
map<int,int> mp;

inline int hsh(){
	int ans = 0;
	for (int i = 0;i < (int)v.size();i ++)ans = ans * 10 + v[i];
	return ans;
}

inline bool ok(vector<int> XC){
	sort(XC.begin(),XC.end());
	return XC[0] + XC[1] > XC[XC.size() - 1];
}

int dfs(int dep){
	// printf("dfs %d\n",dep);
	if (ok(v))return dep;
	int h = hsh();
	if (mp.count(h))return mp[h];
	mp[h] = 0x3f3f3f3f;
	for (int i = 0;i < (int)v.size();i ++){
		for (int j = 0;j < (int)v.size();j ++){
			if (v[i] == v[j])continue;
			int back = v[i];
			v[i] = v[j];
			mp[h] = min(mp[h],dfs(dep + 1));
			v[i] = back;
		}
	}
	return mp[h];
}

int main(){
	scanf("%d",&T);
	while (T --){
		v.clear();
		scanf("%d",&n);
		for (int i = 1;i <= n;i ++){
			int XC;
			scanf("%d",&XC);
			v.push_back(XC);
		}
		printf("%d\n",dfs(0));
	}
	return 0;
}