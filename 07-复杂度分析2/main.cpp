// Problem B: 复杂度分析(Ⅱ)  OJ 1723
// 将嵌套 while 的累计次数化为求和公式，常数时间输出结果。
// i=1; while(i++<n){ j=1; while(j++<i){ k=1; while(k++<j) printf("\n"); } }
// printf 次数 = Σ_{t=1}^{n-1} t(t+1)/2 = n(n-1)(n+1)/6；循环结束后 i+j+k = 3(n+1)
#include <iostream>
#include <string>
using namespace std;

int main() {
    long long n;
    while (cin >> n) {
        // Σ t(t+1)/2 化简后得到 n(n-1)(n+1)/6。
        long long cnt = n * (n - 1) * (n + 1) / 6;
        if (n >= 2) cout << cnt << " " << 3 * (n + 1) << "\n";
        else        cout << cnt << " " << "RANDOM" << "\n";
    }
    return 0;
}
