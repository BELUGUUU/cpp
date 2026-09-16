/*
 * 湖南科技大学 数据结构与算法课程设计   Problem U: 24点游戏(Ⅲ)
 * 题号：1831          语言：C++
 *
 * 题意：输入四个数字，判断能否算出 24；能就输出 T 中「第一个得到 24 的
 *   表达式树」对应的表达式（带 "=24"），否则输出 NO。
 *
 * 思路：直接沿用 Problem T 的枚举次序（i 由小到大、运算符 + - * /、
 *   子集按原位置顺序），一边生成一边求值，遇到第一个等于 24 的就输出并结束。
 *   求值用「分子/分母」的精确分数（约分后比较），这样 1/3 这类除法的
 *   中间结果不会因为浮点误差被误判——这对 24 点题很关键。
 *
 * 编译：g++ -O2 -std=c++11 -o main.exe main.cpp
 */
#include <cstdio>
#include <string>
#include <vector>
#include <cstdlib>
using namespace std;

const char OPS[4] = {'+', '-', '*', '/'};
bool found;
string answer;

long long gcdll(long long a, long long b) {
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    while (b) { long long t = a % b; a = b; b = t; }
    return a ? a : 1;
}

struct Frac {          // 精确分数 num/den，den > 0，且已约分
    long long num, den;
    Frac() { num = 0; den = 1; }
    Frac(long long n) { num = n; den = 1; }
    Frac(long long n, long long d) {
        if (d < 0) { n = -n; d = -d; }
        long long g = gcdll(n, d);
        num = n / g; den = d / g;
    }
};

Frac addF(const Frac &a, const Frac &b) { return Frac(a.num * b.den + b.num * a.den, a.den * b.den); }
Frac subF(const Frac &a, const Frac &b) { return Frac(a.num * b.den - b.num * a.den, a.den * b.den); }
Frac mulF(const Frac &a, const Frac &b) { return Frac(a.num * b.num, a.den * b.den); }
Frac divF(const Frac &a, const Frac &b) { return Frac(a.num * b.den, a.den * b.num); }

struct Item { string e; Frac v; };

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

vector<Item> gen(const vector<int> &pool) {
    vector<Item> res;
    int n = (int)pool.size();
    if (n == 1) {
        Item it;
        char buf[16];
        sprintf(buf, "%d", pool[0]);
        it.e = buf; it.v = Frac(pool[0]);
        res.push_back(it);
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
                vector<Item> ls = gen(L);
                vector<Item> rs = gen(R);
                for (size_t a = 0; a < ls.size(); a++) {
                    for (size_t b = 0; b < rs.size(); b++) {
                        Item it;
                        it.e = "(" + ls[a].e + OPS[o] + rs[b].e + ")";
                        if (OPS[o] == '+') it.v = addF(ls[a].v, rs[b].v);
                        else if (OPS[o] == '-') it.v = subF(ls[a].v, rs[b].v);
                        else if (OPS[o] == '*') it.v = mulF(ls[a].v, rs[b].v);
                        else it.v = divF(ls[a].v, rs[b].v);

                        if (n == 4 && it.v.den != 0 &&
                            it.v.num == 24 * it.v.den) {      // 恰好等于 24
                            answer = it.e;
                            found = true;
                            return res;
                        }
                        res.push_back(it);
                    }
                }
            }
        }
    }
    return res;
}

int main() {
    int v[4];
    while (scanf("%d %d %d %d", &v[0], &v[1], &v[2], &v[3]) == 4) {
        vector<int> pool(v, v + 4);
        found = false;
        answer = "";
        gen(pool);
        if (found) printf("%s=24\n", answer.c_str());
        else printf("NO\n");
    }
    return 0;
}
