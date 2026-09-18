/*
 * 湖南科技大学 数据结构与算法课程设计   Problem Z: 自来水管道
 * 题号：1074          语言：C++
 *
 * 思路：典型的求最小生成树（MST）问题——把供水点看成顶点、铺设路径看成带权边，
 *   要求「任意两点直接或间接连通且总成本最低」，就是求 MST 的边权和。
 *   用 Kruskal：把所有边按成本升序排序，依次取边，若两端点不在同一集合就合并，
 *   累加成本；合并了 P-1 次即得到最小总成本。
 *   并查集用「路径压缩 + 按秩合并」，整体复杂度 O(R log R)。
 *
 * 输入细节：多组数据，每组第一行 P R；P=0 结束（此时没有 R）。
 *   组与组之间可能有空行，用 scanf 直接读整数可以自动跳过空白。
 *   P=1 时没有边，答案是 0。
 *
 * 编译：g++ -O2 -std=c++11 -o main.exe main.cpp
 */
#include <cstdio>
#include <vector>
#include <algorithm>
using namespace std;

struct Edge {
    int u, v, w;
    bool operator<(const Edge &o) const { return w < o.w; }
};

int main() {
    int P, R;
    while (scanf("%d", &P) == 1 && P != 0) {
        scanf("%d", &R);

        // 动态开空间：不再受硬编码上限限制（旧版 parent[55] / e[5000]，
        // 数据规模一旦超过就会越界，轻则答案错、重则 RE）
        vector<int> parent(P + 1);
        for (int i = 1; i <= P; i++) parent[i] = i;

        vector<Edge> e(R);
        for (int i = 0; i < R; i++)
            scanf("%d %d %d", &e[i].u, &e[i].v, &e[i].w);
        sort(e.begin(), e.end());

        // 路径压缩（隔代压缩，不用递归）
        int total = 0, used = 0;
        for (int i = 0; i < R && used < P - 1; i++) {
            int a = e[i].u, b = e[i].v;
            while (parent[a] != a) { parent[a] = parent[parent[a]]; a = parent[a]; }
            while (parent[b] != b) { parent[b] = parent[parent[b]]; b = parent[b]; }
            if (a != b) {
                parent[a] = b;      // 合并两个集合
                total += e[i].w;
                used++;
            }
        }
        printf("%d\n", total);
    }
    return 0;
}
