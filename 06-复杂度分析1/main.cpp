// Problem A: 复杂度分析(Ⅰ)  OJ 1722
// 读入 n，直接用组合公式计算三重循环的输出次数，无需真的执行循环。
// for(i=1;i<n;i++) for(j=1;j<i;j++) for(k=1;k<j;k++) printf("\n");
// printf 次数 = C(n-1,3)；循环结束后 i+j+k = 3n-3 (n>=3)，否则 j/k 未定义输出 RANDOM
#include <iostream>
#include <string>
using namespace std;

int main() {
    long long n;
    while (cin >> n) {
        // C(n-1, 3)：从循环变量可取的值中选出 i > j > k。
        long long a = n - 1, b = n - 2, c = n - 3;
        long long cnt = a * b * c / 6;   // n<4 时乘积里含 0, 结果自然为 0
        if (n >= 3) cout << cnt << " " << 3 * n - 3 << "\n";
        else        cout << cnt << " " << "RANDOM" << "\n";
    }
    return 0;
}
