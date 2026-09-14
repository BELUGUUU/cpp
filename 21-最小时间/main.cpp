/*
 * 湖南科技大学 数据结构与算法课程设计   Problem AA: 最小时间
 * 题号：1075          语言：C++
 *
 * 题意理解：给定铁路网络（边权为通行时间），求出「从城市 1 出发，
 *   在多少时间内能够到达所有城市」——只要最远的那个城市能到达，
 *   其余城市自然都能到达。所以答案 = max{ dist(1, j) | j = 1..n }。
 *
 * 思路：把邻接矩阵补全（输入只给下三角），x 表示无直连，置为无穷大；
 *   用 Floyd 求出所有点对最短路径，再取城市 1 到其余各点距离的最大值。
 *   n<=100，Floyd 的三重循环完全够用。
 *
 * 输入细节：多组数据直到文件结束；每条边权读作字符串，因为可能是 'x'。
 *
 * 编译：g++ -O2 -std=c++11 -o main.exe main.cpp
 */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
using namespace std;

const int INF = 1000000000;
int d[105][105];
char buf[16];

int main() {
    int n;
    while (scanf("%d", &n) == 1) {
        for (int i = 1; i <= n; i++)
            for (int j = 1; j <= n; j++)
                d[i][j] = (i == j ? 0 : INF);

        // 读下三角：第 i 行给出 A(i,1) ... A(i,i-1)
        for (int i = 2; i <= n; i++) {
            for (int j = 1; j <= i - 1; j++) {
                scanf("%s", buf);
                if (buf[0] == 'x' || buf[0] == 'X') continue;   // 无直连
                int t = atoi(buf);
                d[i][j] = d[j][i] = t;      // 铁路是双向的
            }
        }

        // Floyd
        for (int k = 1; k <= n; k++)
            for (int i = 1; i <= n; i++)
                if (d[i][k] < INF)
                    for (int j = 1; j <= n; j++)
                        if (d[i][k] + d[k][j] < d[i][j])
                            d[i][j] = d[i][k] + d[k][j];

        int ans = 0;
        for (int j = 1; j <= n; j++)
            if (d[1][j] > ans) ans = d[1][j];
        printf("%d\n", ans);
    }
    return 0;
}
