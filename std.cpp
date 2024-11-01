#include <bits/stdc++.h>
#define int long long
#define For(i, a, b) for (int i = (a); i <= (b); i ++)
#define foR(i, a, b) for (int i = (a); i >= (b); i --)
using namespace std;
int n, ans, res, siz;
int a[200005], b[200005];
int fa[200005], sz[200005];
int sumji[200005], sumou[200005];
vector <int> v[200005];
map <int, int> m;
int find (int x) {
	if (fa[x] == x) return x;
	return fa[x] = find (fa[x]);
}
void merge (int x, int y) {
	int fx = find (x), fy = find (y);
	if (fx != fy) {
		fa[fx] = fy;
		sz[fy] += sz[fx];
	}
}
int func (int x) {
	if (sz[x] & 1) {
		if (x & 1) return sumji[x + sz[x] - 1] - sumji[x - 1];
		return sumou[x + sz[x] - 1] - sumou[x - 1];
	}
	return max (sumji[x + sz[x] - 1] - sumji[x - 1], sumou[x + sz[x] - 1] - sumou[x - 1]);
}
void solve () {
	res = 0;
	cin >> n;
	For (i, 1, n) a[i] = b[i] = fa[i] = sz[i] = sumji[i] = sumou[i] = 0;
	For (i, 1, n) fa[i] = i;
	For (i, 1, n) {
		cin >> a[i];
		b[i] = a[i];
	}
	sort (a + 1, a + n + 1);
	a[n + 1] = 0;
	int tmp = 0, cnt = 0;
	foR (i, n, 1) {
		if (a[i] != a[i + 1]) {
			tmp = i;
			++ cnt;
		}
		m[a[i] ] = tmp;
	}
	For (i, 1, n) v[m[b[i] ] ].push_back (i);
	For (i, 1, n) {
		if (b[i] == a[n]) {
			if (i & 1) sumji[i] = 1;
			else sumou[i] = 1;
		}
		sumji[i] += sumji[i - 1];
		sumou[i] += sumou[i - 1];
	}
	ans = siz = res = 0;
	int tail = n + 1;
	while (1) {
		while (a[tail - 1] == a[tail]) -- tail;
		-- tail;
		if (tail == 0) break;
		for (int loc : v[m[a[tail] ] ]) {
			sz[loc] = 1;
			fa[loc] = loc;
			res += func (loc);
			++ siz;
			if (loc != 1 && b[loc - 1] >= a[tail] && find (loc - 1) != find (loc) ) {
				res -= func (loc);
				res -= func (find (loc - 1) );
				-- siz;
				siz -= ceil (sz[find (loc - 1)] * 1.0 / 2);
				merge (loc, loc - 1);
				res += func (find (loc) );
				siz += ceil (sz[find (loc - 1)] * 1.0 / 2);
			}
			if (loc != n && b[loc + 1] >= a[tail] && find (loc) != find (loc + 1) ) {
				res -= func (find (loc) );
				res -= func (find (loc + 1) );
				siz -= ceil (sz[find (loc)] * 1.0 / 2);
				siz -= ceil (sz[find (loc + 1)] * 1.0 / 2);
				merge (loc + 1, loc);
				res += func (find (loc) );
				siz += ceil (sz[find (loc)] * 1.0 / 2);
			}
		}
		if (res) ans = max (ans, siz + a[tail] + a[n]);
		else ans = max (ans, siz - 1 + a[tail] + a[n]);
	}
	For (i, 1, n) v[i].clear ();
	m.clear ();
	cout << ans << '\n';
}
signed main () {
	int _ = 1;
	cin >> _;
	while (_ --) solve ();
	return 0;
}