/*
 * 湖南科技大学 数据结构与算法课程设计   Problem T: 24点游戏(Ⅱ)
 * 题号：1830          语言：C++
 *
 * 题意：给出 4 个数字，按规定的次序生成它们能构成的所有表达式树，
 *   每棵输出一行全括号表达式。
 *
 * 生成次序（严格按题目给的递归框架）：
 *   gen(pool)：num = pool 里数字个数
 *     num == 1 时是叶子，直接把数字写出来；
 *     否则，i 从 1 到 num-1（左子树的数字个数）：
 *       运算符按 + - * / 依次枚举；
 *       把 pool 里 i 个数字分给左子树（各子集按原位置次序枚举），其余给右子树；
 *       对左、右子树的每个表达式两两组合。
 *   这样 4 个数字一共生成 7680 个表达式。
 *
 * 编译：g++ -O2 -std=c++11 -o main.exe main.cpp
 */
#include <cstdio>
#include <string>
#include <vector>
using namespace std;

const char OPS[4] = {'+', '-', '*', '/'};

// 枚举 [0,n) 中大小为 k 的「下标」子集（按下标递增，保证「有序」）
// 注意必须按位置（下标）划分数组：数字有重复时按值匹配会拿错位置
void subsets(int n, int k, int start,
             vector<int> &cur, vector<vector<int> > &out) {
    if ((int)cur.size() == k) { out.push_back(cur); return; }
    for (int i = start; i < n; i++) {
        cur.push_back(i);
        subsets(n, k, i + 1, cur, out);
        cur.pop_back();
    }
}

vector<string> gen(const vector<int> &pool) {
    vector<string> res;
    int n = (int)pool.size();
    if (n == 1) {
        char buf[16];
        sprintf(buf, "%d", pool[0]);
        res.push_back(string(buf));
        return res;
    }
    for (int i = 1; i < n; i++) {
        vector<vector<int> > leftSets;      // 存下标
        vector<int> cur;
        subsets(n, i, 0, cur, leftSets);

        for (int o = 0; o < 4; o++) {
            for (size_t s = 0; s < leftSets.size(); s++) {
                // 左子树取 leftSets[s] 这些「位置」上的数字，
                // 右子树取剩余位置上的数字（保持原次序）
                vector<int> L, R;
                vector<bool> used(n, false);
                for (size_t t = 0; t < leftSets[s].size(); t++) {
                    int id = leftSets[s][t];
                    L.push_back(pool[id]);
                    used[id] = true;
                }
                for (int idx = 0; idx < n; idx++)
                    if (!used[idx]) R.push_back(pool[idx]);
                vector<string> ls = gen(L);
                vector<string> rs = gen(R);
                for (size_t a = 0; a < ls.size(); a++)
                    for (size_t b = 0; b < rs.size(); b++)
                        res.push_back("(" + ls[a] + OPS[o] + rs[b] + ")");
            }
        }
    }
    return res;
}

int main() {
    int v[4];
    while (scanf("%d %d %d %d", &v[0], &v[1], &v[2], &v[3]) == 4) {
        vector<int> pool(v, v + 4);
        vector<string> all = gen(pool);
        for (size_t i = 0; i < all.size(); i++)
            printf("%s\n", all[i].c_str());
    }
    return 0;
}
