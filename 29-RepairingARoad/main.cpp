/*
 * 湖南科技大学 数据结构与算法课程设计   Problem AB: Repairing a Road
 * 题号：1044          语言：C++
 *
 * 题意：C 个路口、R 条双向道路，第 i 条路有正常通行时间 v_i 和修理参数 a_i。
 *   你从路口 1 出发的同时，朋友挑恰好一条路修 t 个时间单位（t 是非负实数），
 *   修好之后这条路的通行时间变成 v_i / a_i^t；修理期间不能通过这条路。
 *   求从路口 1 到路口 C 的最短总时间。
 *
 * 思路：
 *   1) 一个最优方案一定是「1 → u 走最短路到达，跨过被修的路 (u,v)，再从 v → C 走最短路」。
 *      用 Floyd 求出全图最短路 d[][]，然后枚举每条路、每个方向。
 *   2) 设到达 u 的时刻为 A = d[1][u]，跨这条路的耗时
 *        f(t) = max(A, t) + v * a^(-t)
 *      —— 你到得早就要等修完（max 里的 t），到得晚就直接走。
 *      · t ≤ A 这一段：A + v·a^(-t) 单调递减，最小值在 t = A；
 *      · t ≥ A 这一段：f'(t) = 1 - v·ln a·a^(-t)，极值点 t* = ln(v·ln a)/ln a（当 v·ln a > 1）。
 *      所以 f 的最小值 = t* + 1/ln a（当 t* > A），否则 = A + v·a^(-A)。
 *      a = 1 时修了也没用，取 t = 0，耗时 A + v。
 *   3) 答案 = min{ d[1][u] + f_min + d[v][C] }，再和「不修路直接走」的 d[1][C] 取小。
 *
 *   复杂度：Floyd O(C^3) + 枚举 O(R)，C ≤ 100、R ≤ 500，非常宽松。
 *
 * 输入：多组数据，每组第一行 C R，随后 R 行 u v v_i a_i（实数），C = R = 0 结束。
 *   下面读的时候把逗号一律换成空格，兼容逗号分隔的版本。
 * 输出：每组一行，保留三位小数。
 *
 * 编译：g++ -O2 -std=c++11 -o main.exe main.cpp
 */
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cmath>
#include <cstring>
#include <algorithm>
using namespace std;

const double INF = 1e18;

int C, R;
double d[105][105];

int    eu[505], ev[505];
double ew[505], ea[505];        // ew = 正常通行时间 v，ea = 修理参数 a

// 到达时刻为 A，跨一条 (v, a) 的路，返回「跨完这条路的时刻」的最小值
double crossMin(double A, double v, double a) {
    if (a > 1.0) {
        double la   = log(a);
        double need = v * la;               // 极值点满足 a^t = v·ln a
        if (need > 1.0) {
            double t = log(need) / la;      // t*
            if (t > A) return t + 1.0 / la; // 最优是「等修完再走」
        }
    }
    return A + v * exp(-log(a) * A);        // 否则最优是 t = A 或 t = 0，等价于直接走
}

static char buf[1 << 20];

int main() {
    size_t len = fread(buf, 1, sizeof(buf) - 1, stdin);
    buf[len] = '\0';
    for (size_t i = 0; i < len; i++)
        if (buf[i] == ',') buf[i] = ' ';    // 逗号当分隔符处理

    char *p = buf;
    while (true) {
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) break;
        char *end;
        C = (int)strtol(p, &end, 10);
        if (end == p) break;
        p = end;
        R = (int)strtol(p, &end, 10);
        if (end == p) break;
        p = end;
        // "0 0" 是每组用例的结束标志（样例里第一组后面还有第二组），
        // 读到它就跳过本组、继续读下一组；整个输入以文件结束为界。
        // 若题目的 0 0 本来就是「全局结束」，那它后面必然没有数据，
        // 效果与 break 相同，两种写法都兼容。
        if (C == 0 && R == 0) continue;

        for (int i = 1; i <= C; i++)
            for (int j = 1; j <= C; j++)
                d[i][j] = (i == j ? 0 : INF);

        for (int k = 0; k < R; k++) {
            int u = (int)strtol(p, &end, 10); p = end;
            int v = (int)strtol(p, &end, 10); p = end;
            double w = strtod(p, &end);       p = end;
            double a = strtod(p, &end);       p = end;
            eu[k] = u; ev[k] = v; ew[k] = w; ea[k] = a;
            if (w < d[u][v]) d[u][v] = d[v][u] = w;
        }

        for (int k = 1; k <= C; k++)                      // Floyd
            for (int i = 1; i <= C; i++) {
                if (d[i][k] >= INF) continue;
                for (int j = 1; j <= C; j++)
                    if (d[i][k] + d[k][j] < d[i][j])
                        d[i][j] = d[i][k] + d[k][j];
            }

        double ans = d[1][C];                             // 完全不修路
        for (int k = 0; k < R; k++) {
            int u = eu[k], v = ev[k];
            if (d[1][u] < INF && d[v][C] < INF)           // 方向 u -> v
                ans = min(ans, crossMin(d[1][u], ew[k], ea[k]) + d[v][C]);
            if (d[1][v] < INF && d[u][C] < INF)           // 方向 v -> u
                ans = min(ans, crossMin(d[1][v], ew[k], ea[k]) + d[u][C]);
        }
        printf("%.3f\n", ans);
    }
    return 0;
}
