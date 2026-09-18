/*
 * 湖南科技大学 数据结构与算法课程设计   Problem V: 24点游戏(Ⅳ)
 * 题号：2007          语言：C++
 *
 * 题意：给出四个数字，求有多少种「互不相同」的解法能算出 24，
 *   并按 Problem T 的顺序输出每个解的第一个表达式。
 *
 * 难点在「不同解法」的判定。题目给了 6 条等价规则：
 *   (1) 加法和乘法满足交换律、结合律
 *   (2) a-b = a+(-b)
 *   (3) a/b = a*(1/b)
 *   (4) a*1 = 1*a = a/1 = a
 *   (5) a/a = 1
 *   (6) a+0 = a-0 = a
 *
 * 思路：把每个表达式规范化成一个字符串，规则相同的表达式规范化后完全相同，
 *   用集合去重即可：
 *   - 加/减：展平成「项的集合」，-b 记作 N(b)，去 0，排序；
 *   - 乘/除：展平成「因子的集合」，b 的倒数记作 I(b)，去 1 和 I(1)，
 *     再把 (x, I(x)) 这样的互逆对消掉，排序；
 *   - 数值用精确分数（分子/分母）表示，除法不做浮点近似。
 *   于是「不同解」= 规范化字符串不同；每个等价类取 T 顺序里第一个表达式输出。
 *   （修正点：与 T/U 同步，枚举时运算符循环提到最外层，保证「T 顺序」一致。）
 *
 * 编译：g++ -O2 -std=c++11 -o main.exe main.cpp
 */
#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
using namespace std;

const char OPS[4] = {'+', '-', '*', '/'};

/* ---------- 精确分数 ---------- */
long long gcdll(long long a, long long b) {
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    while (b) { long long t = a % b; a = b; b = t; }
    return a ? a : 1;
}
struct Frac {
    long long num, den;
    Frac() { num = 0; den = 1; }
    Frac(long long n) { num = n; den = 1; }
    Frac(long long n, long long d) {
        if (d < 0) { n = -n; d = -d; }
        long long g = gcdll(n, d);
        num = n / g; den = d / g;
    }
};

/* ---------- 表达式树 ---------- */
struct Node {
    char op;              // 0 表示叶子
    long long num;        // 叶子的数字
    Node *l, *r;
    Node(char o, Node *a = 0, Node *b = 0) : op(o), num(0), l(a), r(b) {}
    Node(long long n) : op(0), num(n), l(0), r(0) {}
};

/* 规范化用的辅助函数 */
string negStr(const string &c) {                 // N(x)，双层抵消
    if (c.size() > 3 && c[0] == 'N' && c[1] == '(') return c.substr(2, c.size() - 3);
    return "N(" + c + ")";
}
string invStr(const string &c) {                 // I(x)，双层抵消
    if (c.size() > 3 && c[0] == 'I' && c[1] == '(') return c.substr(2, c.size() - 3);
    return "I(" + c + ")";
}

string canon(const Node *x);

void flattenSum(const Node *x, vector<string> &terms, bool negate) {
    if (x->op == '+') { flattenSum(x->l, terms, negate); flattenSum(x->r, terms, negate); }
    else if (x->op == '-') { flattenSum(x->l, terms, negate); flattenSum(x->r, terms, !negate); }
    else {
        string c = canon(x);
        terms.push_back(negate ? negStr(c) : c);
    }
}

void flattenProd(const Node *x, vector<string> &facs, bool invert) {
    if (x->op == '*') { flattenProd(x->l, facs, invert); flattenProd(x->r, facs, invert); }
    else if (x->op == '/') { flattenProd(x->l, facs, invert); flattenProd(x->r, facs, !invert); }
    else {
        string c = canon(x);
        facs.push_back(invert ? invStr(c) : c);
    }
}

// 在因子集合里把互逆对 (x, I(x)) 消掉，并去掉 1 与 I(1)
void reduceFactors(vector<string> &facs) {
    map<string, int> cnt;
    for (size_t i = 0; i < facs.size(); i++) {
        if (facs[i] == "n1" || facs[i] == "I(n1)") continue;   // 规则 (4)
        cnt[facs[i]]++;
    }
    vector<pair<string, int> > items(cnt.begin(), cnt.end());
    for (size_t i = 0; i < items.size(); i++) {
        const string &t = items[i].first;
        int have = cnt[t];
        if (have <= 0) continue;
        string inv = invStr(t);                                // 规则 (5)
        if (cnt.count(inv)) {
            int k = min(have, cnt[inv]);
            cnt[t] -= k;
            cnt[inv] -= k;
        }
    }
    facs.clear();
    for (map<string, int>::iterator it = cnt.begin(); it != cnt.end(); ++it)
        for (int i = 0; i < it->second; i++) facs.push_back(it->first);
}

// 在项集合里把相反项 (x, N(x)) 消掉，并去掉 0 与 -0
void reduceTerms(vector<string> &terms) {
    map<string, int> cnt;
    for (size_t i = 0; i < terms.size(); i++) {
        // 规则 (6)：a+0 = a-0 = a；N(n0) 即 -0，同样由 a-0=a 消去
        if (terms[i] == "n0" || terms[i] == "N(n0)") continue;
        cnt[terms[i]]++;
    }
    vector<pair<string, int> > items(cnt.begin(), cnt.end());
    for (size_t i = 0; i < items.size(); i++) {
        const string &t = items[i].first;
        int have = cnt[t];
        if (have <= 0) continue;
        string neg = negStr(t);
        if (cnt.count(neg)) {
            int k = min(have, cnt[neg]);
            cnt[t] -= k;
            cnt[neg] -= k;
        }
    }
    terms.clear();
    for (map<string, int>::iterator it = cnt.begin(); it != cnt.end(); ++it)
        for (int i = 0; i < it->second; i++) terms.push_back(it->first);
}

string canon(const Node *x) {
    if (x->op == 0) {
        char buf[24];
        sprintf(buf, "n%lld", x->num);
        return string(buf);
    }
    if (x->op == '+' || x->op == '-') {
        vector<string> terms;
        flattenSum(x, terms, false);
        reduceTerms(terms);
        sort(terms.begin(), terms.end());
        if (terms.empty()) return "n0";
        if (terms.size() == 1) return terms[0];
        string s = "S(";
        for (size_t i = 0; i < terms.size(); i++) { if (i) s += ","; s += terms[i]; }
        return s + ")";
    }
    // '*' 或 '/'
    vector<string> facs;
    flattenProd(x, facs, false);
    reduceFactors(facs);
    sort(facs.begin(), facs.end());
    if (facs.empty()) return "n1";
    if (facs.size() == 1) return facs[0];
    string s = "P(";
    for (size_t i = 0; i < facs.size(); i++) { if (i) s += ","; s += facs[i]; }
    return s + ")";
}

/* ---------- 求值 & 中缀式 ---------- */
Frac valueOf(const Node *x) {
    if (x->op == 0) return Frac(x->num);
    Frac a = valueOf(x->l), b = valueOf(x->r);
    if (x->op == '+') return Frac(a.num * b.den + b.num * a.den, a.den * b.den);
    if (x->op == '-') return Frac(a.num * b.den - b.num * a.den, a.den * b.den);
    if (x->op == '*') return Frac(a.num * b.num, a.den * b.den);
    return Frac(a.num * b.den, a.den * b.num);
}
string exprOf(const Node *x) {
    if (x->op == 0) {
        char buf[24];
        sprintf(buf, "%lld", x->num);
        return string(buf);
    }
    return "(" + exprOf(x->l) + string(1, x->op) + exprOf(x->r) + ")";
}
bool divByZero(const Node *x) {
    if (x->op == 0) return false;
    if (x->op == '/' && valueOf(x->r).num == 0) return true;
    return divByZero(x->l) || divByZero(x->r);
}

/* ---------- 枚举 ---------- */
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

vector<Node *> gen(const vector<int> &pool) {
    vector<Node *> res;
    int n = (int)pool.size();
    if (n == 1) { res.push_back(new Node((long long)pool[0])); return res; }

    // 第一步：预先算好「i 个数字给左子树」的所有划分（i 升序、子集按下标序），
    // 左右子树的表达式集合缓存起来给 4 个运算符共用
    struct Split {
        vector<Node *> ls, rs;
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

    // 第二步：运算符在最外层，再依次走 i、子集、左右表达式对（与 Problem T 一致）
    for (int o = 0; o < 4; o++) {
        for (int i = 1; i < n; i++) {
            for (size_t s = 0; s < splits[i].size(); s++) {
                const vector<Node *> &ls = splits[i][s].ls;
                const vector<Node *> &rs = splits[i][s].rs;
                for (size_t a = 0; a < ls.size(); a++)
                    for (size_t b = 0; b < rs.size(); b++)
                        res.push_back(new Node(OPS[o], ls[a], rs[b]));
            }
        }
    }
    return res;
}

int main() {
    int v[4];
    while (scanf("%d %d %d %d", &v[0], &v[1], &v[2], &v[3]) == 4) {
        vector<int> pool(v, v + 4);
        vector<Node *> all = gen(pool);

        set<string> seen;
        vector<string> answers;
        for (size_t i = 0; i < all.size(); i++) {
            Node *t = all[i];
            if (divByZero(t)) continue;
            Frac val = valueOf(t);
            if (!(val.den != 0 && val.num == 24 * val.den)) continue;   // 不等于 24
            string key = canon(t);
            if (seen.find(key) == seen.end()) {
                seen.insert(key);
                answers.push_back(exprOf(t));      // 取该等价类里第一个表达式
            }
        }

        printf("%d\n", (int)answers.size());
        for (size_t i = 0; i < answers.size(); i++)
            printf("%s=24\n", answers[i].c_str());
    }
    return 0;
}
