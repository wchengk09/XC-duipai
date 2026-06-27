#include <iostream>
using namespace std;
int main() {
    long long a, b;
    cin >> a >> b;
    // 故意除零触发 RE
    int zero = 0;
    cout << (a + b) / zero << endl;
    return 0;
}
