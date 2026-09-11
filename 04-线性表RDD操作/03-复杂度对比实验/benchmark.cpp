// ============================================================================
//  复杂度对比实验：R/D 操作的四种实现
// ----------------------------------------------------------------------------
//  同一组数据、同一段操作串，分别用四种存储/策略跑一遍，比较耗时增长趋势：
//
//    A. 双向链表 + rev 标志        每次 R/D 均 O(1)
//    B. 双向链表 + 物理逆置        每次 R 真去逐结点交换 prev/next，O(n)
//    C. 顺序表   + 下标窗口 + 标志  每次 R/D 均 O(1)
//    D. 顺序表   + 物理删除(移位)   每次 D 用 erase 搬元素，O(n)
//
//  四者输出必须完全一致（代码里做了校验），差别只在时间上。
//  B 和 D 就是"按字面直译题目"的写法，本实验说明它们为什么会被卡死。
//
//  编译:  g++ -O2 -std=c++11 -o benchmark.exe benchmark.cpp
//  运行:  ./benchmark.exe
// ============================================================================

#include <cstdio>
#include <string>
#include <vector>
#include <chrono>
#include <algorithm>

using std::string;
using std::vector;

// ------------------------------- 计时工具 -------------------------------
typedef std::chrono::steady_clock Clock;

static double msSince(const Clock::time_point &t0) {
    Clock::time_point t1 = Clock::now();
    return std::chrono::duration<double, std::milli>(t1 - t0).count();
}

// ------------------------- 实现 A：双向链表 + 标志 -------------------------
class DListFlag {
public:
    DListFlag() : head_(nullptr), tail_(nullptr), size_(0), rev_(false) {}
    ~DListFlag() { clear(); }

    void build(const vector<int> &a) {
        clear();
        for (size_t i = 0; i < a.size(); i++) { pushBack(a[i]); }
    }
    void reverse() { rev_ = !rev_; }                       // O(1)
    bool removeFirst() {                                   // O(1)
        if (size_ == 0) { return false; }
        if (!rev_) { Node *p = head_; head_ = p->next; if (head_) head_->prev = nullptr; else tail_ = nullptr; delete p; }
        else       { Node *p = tail_; tail_ = p->prev; if (tail_) tail_->next = nullptr; else head_ = nullptr; delete p; }
        --size_;
        return true;
    }
    string toString() const {
        string s; s.reserve((size_t)size_ * 4 + 2); s.push_back('[');
        if (!rev_) { for (Node *p = head_; p; p = p->next) { num(s, p->val, p->next != nullptr); } }
        else       { for (Node *p = tail_; p; p = p->prev) { num(s, p->val, p->prev != nullptr); } }
        s.push_back(']');
        return s;
    }
private:
    struct Node {
        int val; Node *prev, *next;
        explicit Node(int v) : val(v), prev(nullptr), next(nullptr) {}
    };
    Node *head_, *tail_; int size_; bool rev_;

    void pushBack(int v) {
        Node *p = new Node(v); p->prev = tail_;
        if (tail_) tail_->next = p; else head_ = p;
        tail_ = p; ++size_;
    }
    void clear() {
        Node *p = head_;
        while (p) { Node *n = p->next; delete p; p = n; }
        head_ = tail_ = nullptr; size_ = 0; rev_ = false;
    }
    static void num(string &s, int v, bool more) {
        char b[8]; int k = 0;
        if (v == 0) { b[k++] = '0'; }
        while (v > 0) { b[k++] = (char)('0' + v % 10); v /= 10; }
        while (k-- > 0) { s.push_back(b[k]); }
        if (more) { s.push_back(','); }
    }
};

// --------------------- 实现 B：双向链表 + 物理逆置（反面） ---------------------
class DListNaive {
public:
    DListNaive() : head_(nullptr), tail_(nullptr), size_(0) {}
    ~DListNaive() { clear(); }

    void build(const vector<int> &a) {
        clear();
        for (size_t i = 0; i < a.size(); i++) { pushBack(a[i]); }
    }
    // 真的逆置：逐结点交换 prev/next，再交换首尾指针 —— O(n)
    void reverse() {
        Node *p = head_;
        while (p) {
            Node *n = p->next;
            p->next = p->prev;
            p->prev = n;
            p = n;
        }
        Node *t = head_;
        head_ = tail_;
        tail_ = t;
    }
    bool removeFirst() {                                   // O(1)
        if (size_ == 0) { return false; }
        Node *p = head_;
        head_ = p->next;
        if (head_) head_->prev = nullptr; else tail_ = nullptr;
        delete p; --size_;
        return true;
    }
    string toString() const {
        string s; s.reserve((size_t)size_ * 4 + 2); s.push_back('[');
        for (Node *p = head_; p; p = p->next) { num(s, p->val, p->next != nullptr); }
        s.push_back(']');
        return s;
    }
private:
    struct Node {
        int val; Node *prev, *next;
        explicit Node(int v) : val(v), prev(nullptr), next(nullptr) {}
    };
    Node *head_, *tail_; int size_;

    void pushBack(int v) {
        Node *p = new Node(v); p->prev = tail_;
        if (tail_) tail_->next = p; else head_ = p;
        tail_ = p; ++size_;
    }
    void clear() {
        Node *p = head_;
        while (p) { Node *n = p->next; delete p; p = n; }
        head_ = tail_ = nullptr; size_ = 0;
    }
    static void num(string &s, int v, bool more) {
        char b[8]; int k = 0;
        if (v == 0) { b[k++] = '0'; }
        while (v > 0) { b[k++] = (char)('0' + v % 10); v /= 10; }
        while (k-- > 0) { s.push_back(b[k]); }
        if (more) { s.push_back(','); }
    }
};

// ---------------------- 实现 C：顺序表 + 下标窗口 + 标志 ----------------------
class ArrayWindow {
public:
    void build(const vector<int> &a) { a_ = a; lo_ = 0; hi_ = (int)a_.size() - 1; rev_ = false; }
    void reverse() { rev_ = !rev_; }                       // O(1)
    bool removeFirst() {                                   // O(1)
        if (lo_ > hi_) { return false; }
        if (!rev_) { ++lo_; } else { --hi_; }
        return true;
    }
    string toString() const {
        if (lo_ > hi_) { return string("[]"); }
        string s; s.reserve((size_t)(hi_ - lo_ + 1) * 4 + 2); s.push_back('[');
        if (!rev_) { for (int i = lo_; i <= hi_; i++) { num(s, a_[(size_t)i], i < hi_); } }
        else       { for (int i = hi_; i >= lo_; i--) { num(s, a_[(size_t)i], i > lo_); } }
        s.push_back(']');
        return s;
    }
private:
    vector<int> a_; int lo_, hi_; bool rev_;
    static void num(string &s, int v, bool more) {
        char b[8]; int k = 0;
        if (v == 0) { b[k++] = '0'; }
        while (v > 0) { b[k++] = (char)('0' + v % 10); v /= 10; }
        while (k-- > 0) { s.push_back(b[k]); }
        if (more) { s.push_back(','); }
    }
};

// -------------------- 实现 D：顺序表 + 物理删除（移位，反面） --------------------
class ArrayErase {
public:
    void build(const vector<int> &a) { a_ = a; rev_ = false; }
    void reverse() { rev_ = !rev_; }                       // 仍然只翻标志，
                                                           // 否则逆置本身又成 O(n)
    bool removeFirst() {                                   // O(n)：要搬元素
        if (a_.empty()) { return false; }
        if (!rev_) {
            a_.erase(a_.begin());                          // 移位覆盖
        } else {
            a_.pop_back();                                 // 逻辑头在尾部，直接删尾
        }
        return true;
    }
    string toString() const {
        string s; s.reserve(a_.size() * 4 + 2); s.push_back('[');
        if (!rev_) { for (size_t i = 0; i < a_.size(); i++) { num(s, a_[i], i + 1 < a_.size()); } }
        else       { for (size_t i = a_.size(); i-- > 0;) { num(s, a_[i], i > 0); } }
        s.push_back(']');
        return s;
    }
private:
    vector<int> a_; bool rev_;
    static void num(string &s, int v, bool more) {
        char b[8]; int k = 0;
        if (v == 0) { b[k++] = '0'; }
        while (v > 0) { b[k++] = (char)('0' + v % 10); v /= 10; }
        while (k-- > 0) { s.push_back(b[k]); }
        if (more) { s.push_back(','); }
    }
};

// ------------------------------ 统一驱动器 ------------------------------
//  对任意实现跑一遍操作串，返回输出；中途空表删除则返回 "error"
template <class List>
static string runSim(List &lst, const vector<int> &init, const string &S) {
    lst.build(init);
    for (size_t i = 0; i < S.size(); i++) {
        char c = S[i];
        if (c == 'R') { lst.reverse(); }
        else if (c == 'D') { if (!lst.removeFirst()) { return string("error"); } }
    }
    return lst.toString();
}

// 数据生成：自己写 xorshift，保证跨机器可复现
static unsigned int g_seed = 88172645u;
static unsigned int rnd() {
    g_seed ^= g_seed << 13; g_seed ^= g_seed >> 17; g_seed ^= g_seed << 5;
    return g_seed;
}

int main() {
    // 规模逐步翻倍，观察耗时增长趋势（理论上 A/C 线性、B/D 二次）
    const int sizes[] = { 2000, 4000, 8000, 12000 };
    const int SCALE_CNT = (int)(sizeof(sizes) / sizeof(sizes[0]));

    printf("========================================================================\n");
    printf(" R/D 操作四种实现性能对比          规模 n 与操作串长度 |S| = 4n\n");
    printf(" 操作串组成: 80%% 'R' + 20%% 'D'（D 不超过 n，避免提前 error）\n");
    printf("========================================================================\n\n");

    printf("%-8s %-9s %-14s %-14s %-14s %-14s %-8s\n",
           "n", "|S|", "A双链+标志", "B双链+物理逆置", "C顺序+窗口", "D顺序+物理删除", "输出一致");
    printf("%-8s %-9s %-14s %-14s %-14s %-14s %-8s\n",
           "--------", "---------", "--------------", "--------------", "--------------", "--------------", "--------");

    double res[SCALE_CNT][4];          // [规模序号][实现 A/B/C/D]

    for (int k = 0; k < SCALE_CNT; k++) {
        int n = sizes[k];
        int slen = 4 * n;

        // 初始表（n 个 1..100 的随机数）
        vector<int> init((size_t)n);
        for (int i = 0; i < n; i++) { init[(size_t)i] = (int)(rnd() % 100) + 1; }

        // 操作串：80% R、20% D
        string S;
        S.reserve((size_t)slen);
        for (int i = 0; i < slen; i++) { S.push_back((rnd() % 100 < 80) ? 'R' : 'D'); }

        double tA, tB, tC, tD;
        string outA, outB, outC, outD;

        { DListFlag l;  Clock::time_point t0 = Clock::now(); outA = runSim(l, init, S); tA = msSince(t0); }
        { DListNaive l; Clock::time_point t0 = Clock::now(); outB = runSim(l, init, S); tB = msSince(t0); }
        { ArrayWindow l;Clock::time_point t0 = Clock::now(); outC = runSim(l, init, S); tC = msSince(t0); }
        { ArrayErase l; Clock::time_point t0 = Clock::now(); outD = runSim(l, init, S); tD = msSince(t0); }

        bool same = (outA == outB) && (outA == outC) && (outA == outD);

        res[k][0] = tA; res[k][1] = tB; res[k][2] = tC; res[k][3] = tD;

        printf("%-8d %-9d %-14.2f %-14.2f %-14.2f %-14.2f %-8s\n",
               n, slen, tA, tB, tC, tD, same ? "是" : "否!");
    }

    // 用实测数据算增长倍数，避免写死理论值对不上
    printf("\n------------------------------------------------------------------------\n");
    printf("增长倍数（n 从 %d 增至 %d，规模共 %.1f 倍）：\n",
           sizes[0], sizes[SCALE_CNT - 1],
           (double)sizes[SCALE_CNT - 1] / (double)sizes[0]);
    printf("  A 双向链表+标志     %7.1f 倍\n", res[SCALE_CNT - 1][0] / res[0][0]);
    printf("  B 双向链表+物理逆置 %7.1f 倍\n", res[SCALE_CNT - 1][1] / res[0][1]);
    printf("  C 顺序表+窗口       %7.1f 倍  (绝对耗时极小，易受计时噪声影响)\n",
           res[SCALE_CNT - 1][2] / res[0][2]);
    printf("  D 顺序表+物理删除   %7.1f 倍\n", res[SCALE_CNT - 1][3] / res[0][3]);
    printf("\n");
    printf("再看一组更稳的对比（n 从 %d 增至 %d，规模共 3.0 倍，避开极小基准的计时噪声）：\n",
           sizes[1], sizes[SCALE_CNT - 1]);
    printf("  A 双向链表+标志     %7.1f 倍   （线性应 ≈ 3）\n", res[SCALE_CNT - 1][0] / res[1][0]);
    printf("  B 双向链表+物理逆置 %7.1f 倍   （二次应 ≈ 9）\n", res[SCALE_CNT - 1][1] / res[1][1]);
    printf("  C 顺序表+窗口       %7.1f 倍   （线性应 ≈ 3）\n", res[SCALE_CNT - 1][2] / res[1][2]);
    printf("  D 顺序表+物理删除   %7.1f 倍   （二次应 ≈ 9）\n", res[SCALE_CNT - 1][3] / res[1][3]);
    printf("\n");
    printf("结论：A/C 的增长倍数与规模增长倍数同阶（线性），\n");
    printf("      B/D 的增长倍数远高于规模增长倍数，呈二次增长。\n");
    printf("      外推到 OJ 上限 n = |S| = 1e5（再放大 8 倍规模），\n");
    printf("      B/D 将需要约 1e10 次基本操作，而 1 秒大致只能承受 1e8~1e9 次，\n");
    printf("      必然 TLE —— 这就是必须用「逆转标志」而不是真逆置的原因。\n");
    printf("------------------------------------------------------------------------\n");
    return 0;
}
