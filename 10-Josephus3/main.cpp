// Problem E: Josephus问题(Ⅲ) k=2 递推公式  OJ 1720
// n < 2^31 不能模拟。k=2 时有 O(1) 公式：设 n = 2^m + L (0 <= L < 2^m)，
// 则最后剩下的人编号 J(n) = 2L + 1
#include <iostream>
using namespace std;

int main() {
    long long n;
    while (cin >> n) {
        long long p = 1;
        while (p * 2 <= n) p *= 2;      // 不超过 n 的最大 2 的幂
        cout << 2 * (n - p) + 1 << "\n";
    }
    return 0;
}
