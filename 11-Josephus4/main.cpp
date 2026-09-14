// Problem F: Josephus问题(IV) 通用递推+跳跃优化  OJ 1721
// n < 2^31, k <= 100。递推 f(1)=0, f(i) = (f(i-1)+k) % i，答案 f(n)+1。
// 每组数据 O(n) 会超时：当 f + (k-1)*t < i 时可连续递推 t 步不用取模，一次跳完
#include <iostream>
using namespace std;
typedef long long ll;

int main() {
    ll n, k;
    while (cin >> n >> k) {
        if (k == 1) { cout << n << "\n"; continue; }   // k=1 时剩下的一定是 n 号
        ll f = 0;      // f(1)
        ll i = 1;
        while (i < n) {
            // 最长可无取模连跳步数: f + (k-1)*s < i  =>  s <= (i-1-f)/(k-1)
            ll t = (i - 1 - f) / (k - 1);
            if (t < 1) t = 1;
            if (i + t > n) t = n - i;
            f = (f + k * t) % (i + t);
            i += t;
        }
        cout << f + 1 << "\n";
    }
    return 0;
}
