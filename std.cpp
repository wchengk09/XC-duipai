#include <bits/stdc++.h>
using namespace std;

vector<int> v;

template <class T>
	void clear(T &x){
		T y;
		swap(x,y);
	}

int main(){
	#ifndef kevin
	freopen("god.in","r",stdin);
	freopen("god.out","w",stdout);
	#endif
	int T;
	scanf("%d",&T);
	while (T --){
		int n,q;
		scanf("%d%d",&n,&q);
		clear(v);
		for (int i = 1;i <= n;i ++){
			int x;
			scanf("%d",&x);
			v.push_back(x);
		}
		printf("0\n");
		while (q --){
			int opt;
			int x;
			scanf("%d%d",&opt,&x);
			if (opt == 0)v.insert(v.begin(),x);
			else v.push_back(x);
			if (v.size() == 2){
				if (v[0] == v[1])printf("1\n");
				else printf("0\n");
			}else{
				if (v[0] + v[1] == v[2])printf("1\n");
				else printf("0\n");
			}
		}
	}
	return 0;
}