/*
 * 湖南科技大学 数据结构与算法课程设计   Problem BB: 2010省赛 Repairing a Road
 * 题号：2010          语言：C++
 *
 * 题意：C 个路口、R 条双向路，要从 1 号走到 C 号。每条路有原通行时间 v 与
 *   修复系数 a。你只有一次机会请朋友修一条路：若他修了 t 个单位时间，
 *   修好后这条路的花费变成 v * a^(-t)（t 越大越短，但修的时候不能走）。
 *   求能够到达 C 的最早时间。
 *
 * 思路（三分 + 最短路）：
 *   在修某条边 (u,v) 并沿 u->v 走的情况下，
 *     到达 u 的最早时间为 d1[u]（以原时间算的最短路）；
 *     通过这条边的时间为 v * a^(-t)，其中 t 是修复时长；
 *     开始通过的时刻是 max(d1[u], t)（修的时候不允许通过）；
 *     通过之后用 d2[v]（v 到 C 的最短路）继续走。
 *   总时间 f(t) = max(d1[u], t) + v*a^(-t) + d2[v]。
 *     当 t <= d1[u] 时 f 随 t 递减，最优取 t = d1[u]；
 *     当 t > d1[u] 时，g(t) = t + v*a^(-t) 是凸函数，
 *       解析求导：g'(t)=0 → a^(-t) = 1/(v*ln a) → t* = ln(v*ln a)/ln a，
 *       若 t* 落在 t > d1[u] 范围内则取 t*。
 *   枚举每条边两个方向取最小值，并与「完全不修」的 d1[C] 比较。
 *
 * 输入：多组数据，每组首行 C R（2<=C<=100，1<=R<=500），随后 R 行
 *   每行 x y length a；以 "0 0" 结束。
 * 输出：最小时间，保留 3 位小数。
 *
 * 编译：g++ -O2 -std=c++11 -o main.exe main.cpp
 */
#include <cstdio>
#include <cmath>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;

const double INF = 1e18;

struct Road { int u, v; double len, rate; };
vector<Road> roads;
vector<pair<int, double> > g[105];
double d1[105], d2[105];
int C, R;

// 从 src 出发的单源最短路（Dijkstra，n 很小直接用邻接表 + 优先队列）
void dijkstra(int src, double *dist) {
    for (int i = 1; i <= C; i++) dist[i] = INF;
    dist[src] = 0;
    priority_queue<pair<double, int>, vector<pair<double, int> >,
                   greater<pair<double, int> > > pq;
    pq.push(make_pair(0.0, src));
    while (!pq.empty()) {
        pair<double, int> top = pq.top(); pq.pop();
        double d = top.first; int u = top.second;
        if (d > dist[u] + 1e-12) continue;
        for (size_t i = 0; i < g[u].size(); i++) {
            int v = g[u][i].first;
            double nd = d + g[u][i].second;
            if (nd < dist[v] - 1e-12) {
                dist[v] = nd;
                pq.push(make_pair(nd, v));
            }
        }
    }
}

// 计算「到达 u 用时到达时刻 base、再走这条修好的边、然后到 C」的最优总时间
double bestCross(double base, double len, double rate, double tail) {
    double best;
    // 情形一：修复时间不超过 base，一到就能走（此时 t 越小越好，取下界）
    double t0 = base;                     // t = base
    best = max(base, t0) + len * pow(rate, -t0) + tail;

    // 情形二：t >= base，用凸函数解析极值点
    if (rate > 1.0 + 1e-12) {
        double lna = log(rate);
        double tStar = log(len * lna) / lna;   // g'(t)=0 的解
        if (tStar > base) {
            double val = tStar + len * pow(rate, -tStar) + tail;
            if (val < best) best = val;
        }
    }
    return best;
}

int main() {
    while (scanf("%d %d", &C, &R) == 2) {
        if (C == 0 && R == 0) break;

        roads.clear();
        for (int i = 1; i <= C; i++) g[i].clear();
        for (int i = 0; i < R; i++) {
            Road r;
            scanf("%d %d %lf %lf", &r.u, &r.v, &r.len, &r.rate);
            roads.push_back(r);
            g[r.u].push_back(make_pair(r.v, r.len));
            g[r.v].push_back(make_pair(r.u, r.len));
        }

        dijkstra(1, d1);      // 1 到各点
        dijkstra(C, d2);      // 各点到 C（无向图，等价于从 C 出发）

        double ans = d1[C];   // 不修路
        for (size_t i = 0; i < roads.size(); i++) {
            Road &r = roads[i];
            // 沿 u -> v 走
            if (d1[r.u] < INF / 2 && d2[r.v] < INF / 2) {
                double c = bestCross(d1[r.u], r.len, r.rate, d2[r.v]);
                if (c < ans) ans = c;
            }
            // 沿 v -> u 走
            if (d1[r.v] < INF / 2 && d2[r.u] < INF / 2) {
                double c = bestCross(d1[r.v], r.len, r.rate, d2[r.u]);
                if (c < ans) ans = c;
            }
        }
        printf("%.3f\n", ans);
    }
    return 0;
}
