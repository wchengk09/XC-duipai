#include <bits/stdc++.h>
using namespace std;
const int MAXN = 205;
const int MAXK = 3005;
const int B = 80;
const int QUEUE = 3e7 + 5;
int T,n,k;
char str[MAXN];
int trie[MAXN][2],fail[MAXN];
int sum[MAXN],dep[MAXN];
bool vis[MAXN][MAXK][B][2];
pair<int,bool> pre[QUEUE];

struct Data{
	short j,k,s;
	bool sub;
	Data(){}
	Data(short j,short k,short s,bool sub):j(j),k(k),s(s),sub(sub){}
};
Data q[QUEUE];
int hd,tl;

void push(Data dt){q[++tl] = dt;}
void pop(){++hd;}

void ACAM(){
	for (int i = 0;i < n;i ++){
		trie[i][str[i + 1] - '0'] = i + 1;
		sum[i] = (str[i + 1] == '1');
	}
	fail[0] = 0;
	fail[trie[0][str[1] - '0']] = 0;
	for (int i = 1;i <= n;i ++){
		for (int j = 0;j <= 1;j ++){
			if (trie[i][j])fail[trie[i][j]] = trie[fail[i]][j];
			else trie[i][j] = trie[fail[i]][j];
		}
	}
	for (int i = 1;i <= n;i ++){
		sum[i] += sum[fail[i]];
		dep[i] = dep[fail[i]] + 1;
	}
}

void clear(){
	memset(trie,0,sizeof(trie));
	memset(fail,0,sizeof(fail));
	memset(vis,0,sizeof(vis));
	memset(sum,0,sizeof(sum));
	memset(dep,0,sizeof(dep));
}

int main(){
	scanf("%*d%d",&T);
	while (T --){
		scanf("%d%d%s",&n,&k,str + 1);
		ACAM();
		hd = 1,tl = 0;
		push(Data(0,0,0,0));
		vis[0][0][0][0] = true;
		while (hd <= tl){
			Data cur = q[hd ++];
			// printf("%d %d %d %d\n",cur.j,cur.k,cur.s,cur.sub);
			for (int x = 0;x <= 1;x ++){
				Data nxt;
				nxt.j = trie[cur.j][x];
				nxt.s = cur.s;
				if (x == 0)nxt.s += sum[cur.j];
				nxt.k = cur.k + nxt.s + dep[nxt.j] - (nxt.j == n);
				nxt.sub = cur.sub || nxt.j == n;
				// printf("  %d -> %d %d %d %d\n",x,nxt.j,nxt.k,nxt.s,nxt.sub);
				if (nxt.k <= k && !vis[nxt.j][nxt.k][nxt.s][nxt.sub]){
					vis[nxt.j][nxt.k][nxt.s][nxt.sub] = true;
					push(nxt);
					pre[tl] = {hd - 1,x};
				}
				if (nxt.k == k && nxt.sub)break;
			}
			if (q[tl].k == k && q[tl].sub)break;
		}
		if (hd > tl)printf("-1\n");
		else{
			string out;
			int ptr = tl;
			while (ptr > 1){
				out += pre[ptr].second + '0';
				ptr = pre[ptr].first;
			}
			reverse(out.begin(),out.end());
			printf("%s\n",out.c_str());
		}

		clear();
	}
	return 0;
}