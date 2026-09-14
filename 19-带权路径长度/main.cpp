/*
 * 湖南科技大学 数据结构与算法课程设计   Problem Y: 带权路径长度
 * 题号：1832          语言：C++
 *
 * 思路：哈夫曼树的构造过程本身就在累加带权路径长度。
 *   每次取出两个最小权值合并，把「合并后的权值」累加到总和里，
 *   合并 n-1 次后总和恰好就是整棵哈夫曼树的 WPL。
 *   用小根堆（优先队列）维护，复杂度 O(n log n)。
 *
 * 溢出处理：n<=1e5、权值<=1e9，WPL 最大量级约 1e5*1e9*44 ≈ 4.4e15，
 *   用 long long 足够；最后对 1000000007 取模输出。
 *
 * 编译：g++ -O2 -std=c++11 -o main.exe main.cpp
 */
#include <cstdio>
#include <queue>
#include <vector>
using namespace std;

int main() {
    int n;
    while (scanf("%d", &n) == 1) {
        priority_queue<long long, vector<long long>, greater<long long> > pq;
        for (int i = 0; i < n; i++) {
            long long w;
            scanf("%lld", &w);
            pq.push(w);
        }
        long long wpl = 0;
        while (pq.size() > 1) {
            long long a = pq.top(); pq.pop();
            long long b = pq.top(); pq.pop();
            long long s = a + b;
            wpl += s;          // 合并代价即该内部结点的权值，全部加起来就是 WPL
            pq.push(s);
        }
        printf("%lld\n", wpl % 1000000007LL);
    }
    return 0;
}
