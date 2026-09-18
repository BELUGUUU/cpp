/*
 * 湖南科技大学 数据结构与算法课程设计   Problem T: 24点游戏(Ⅱ)
 * 题号：1830          语言：C++
 *
 * 题意：给出 4 个数字，按规定的次序生成它们能构成的所有表达式树，
 *   每棵输出一行全括号表达式（4 个数字共 7680 个）。
 *
 * 生成次序（修正版）：
 *   gen(pool)：num = pool 里数字个数
 *     num == 1 时是叶子，直接把数字写出来；
 *     否则按「运算符 → 左子树数字个数 i → 左子树子集 → 左右两两组合」展开：
 *       运算符按 + - * / 在最外层依次枚举；
 *       i 从 1 到 num-1，表示分给左子树的数字个数；
 *       子集按「原位置（下标）」递增枚举，其余数字按原次序给右子树。
 *   —— 修正点：旧版把「i → 子集」放在运算符外层，前 12 行与样例一致、
 *      第 25 行起才与题面次序分岔（样例输出只贴了前 12 行，区分不出来）。
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

    // 第一步：先算好「i 个数字给左子树」的所有划分（i 升序、子集按下标序），
    // 左右子树的表达式集合缓存起来给 4 个运算符共用，避免重复递归
    struct Split {
        vector<string> ls, rs;
    };
    vector<vector<Split> > splits(n);
    for (int i = 1; i < n; i++) {
        vector<vector<int> > leftSets;      // 存下标
        vector<int> cur;
        subsets(n, i, 0, cur, leftSets);

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

            Split sp;
            sp.ls = gen(L);
            sp.rs = gen(R);
            splits[i].push_back(sp);
        }
    }

    // 第二步：运算符在最外层，再依次走 i、子集、左右表达式对
    for (int o = 0; o < 4; o++) {
        for (int i = 1; i < n; i++) {
            for (size_t s = 0; s < splits[i].size(); s++) {
                const vector<string> &ls = splits[i][s].ls;
                const vector<string> &rs = splits[i][s].rs;
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
