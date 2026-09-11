// ============================================================================
//  线性表 R / D 操作  ——  版本一：双向链表 + 逆转标志
// ----------------------------------------------------------------------------
//  题目要求
//    (1) 设计一个 ADT
//    (2) 至少实现两个版本，其中一种使用【双向链表】   <-- 本文件
//    操作串 S 只含 'R'(逆转整表) 与 'D'(删除表中第一个元素)，
//    若对空表执行 'D' 则输出 error。
//    |S| <= 100000，n <= 100000，元素值 1..100。
//
//  ★ 关键设计：绝不真的"逆转"表
//      R 若老老实实逐结点交换 prev/next，单次就是 O(n)。而 |S| 可达 1e5、
//      每组还可能多次 R，最坏 O(n*|S|) = 1e10，必然超时。
//      正确做法是加一个布尔量 rev，记录"逻辑顺序是否与物理顺序相反"：
//          R  : rev = !rev                                 O(1)
//          D  : rev 为假 -> 删物理头；为真 -> 删物理尾      O(1)
//          输出: rev 为假 -> 从 head 正序遍历；为真 -> 从 tail 逆序遍历
//
//  ★ 为什么必须用【双向】链表
//      rev 为真时，"表中第一个元素"落在物理尾结点上，删除它之后要把 tail_
//      指回它的【前驱】。双向链表靠 prev 一步到位；单链表只能从头扫到尾，
//      尾删退化成 O(n)。所以本题"必须用双向链表"不只是形式要求，它正是
//      O(1) 双端删除的前提。
//
//  复杂度
//      建表 O(n)；扫描操作串 O(|S|)；输出 O(n)；额外空间 O(n)（链表本身）。
//      整体与 |S|、n 成线性，1e5 规模稳稳过。
// ============================================================================

#include <cstdio>
#include <string>
#include <vector>
#include <iostream>

// ============================================================================
//  ADT  LinearList  ——  只描述"能做什么"，不暴露"怎么存"
// ============================================================================
//  数据对象 : D = { a_i | a_i ∈ [1,100], i = 1..n, n >= 0 }
//  数据关系 : R = { <a_(i-1), a_i> }  （相邻元素前后相继，有序）
//  基本操作 :
//      build(a)       用初始序列 a 建表              O(n)
//      reverse()      'R'：逆转整个表                O(1)
//      removeFirst()  'D'：删表中第一个元素；空表返回 false
//      empty()        判空
//      toString()     按 "[e1,e2,...]" 输出          O(n)
// ============================================================================
class LinearList {
public:
    virtual ~LinearList() {}

    virtual void        build(const std::vector<int> &a) = 0;
    virtual void        reverse() = 0;
    virtual bool        removeFirst() = 0;
    virtual bool        empty() const = 0;
    virtual std::string toString() const = 0;
};

// ----------------------------------------------------------------------------
//  实现一：双向链表
// ----------------------------------------------------------------------------
class DLinkedList : public LinearList {
private:
    struct Node {
        int   val;
        Node *prev;
        Node *next;
        explicit Node(int v) : val(v), prev(nullptr), next(nullptr) {}
    };

    Node *head_;      // 物理头结点
    Node *tail_;      // 物理尾结点
    int   size_;
    bool  rev_;       // 逻辑顺序是否与物理顺序相反

    // 把 v 接到物理尾部
    void pushBack(int v) {
        Node *p = new Node(v);
        p->prev = tail_;
        if (tail_) {
            tail_->next = p;
        } else {
            head_ = p;
        }
        tail_ = p;
        ++size_;
    }

    // 整数转字符串追加到 out（元素 <= 100，自己转比 sprintf 快）
    static void appendNum(std::string &out, int v, bool more) {
        char buf[8];
        int  k = 0;
        if (v == 0) {
            buf[k++] = '0';
        }
        while (v > 0) {
            buf[k++] = (char)('0' + v % 10);
            v /= 10;
        }
        while (k-- > 0) {
            out.push_back(buf[k]);
        }
        if (more) {
            out.push_back(',');
        }
    }

public:
    DLinkedList() : head_(nullptr), tail_(nullptr), size_(0), rev_(false) {}

    ~DLinkedList() {                       // 迭代释放，避免深链递归爆栈
        Node *p = head_;
        while (p) {
            Node *n = p->next;
            delete p;
            p = n;
        }
    }

    void build(const std::vector<int> &a) {
        for (size_t i = 0; i < a.size(); i++) {
            pushBack(a[i]);
        }
    }

    void reverse() { rev_ = !rev_; }       // 只翻标志，O(1)

    bool removeFirst() {
        if (size_ == 0) {
            return false;                  // 空表删除 -> error
        }
        if (!rev_) {
            // 逻辑头 == 物理头
            Node *p = head_;
            head_ = p->next;
            if (head_) head_->prev = nullptr;
            else       tail_ = nullptr;
            delete p;
        } else {
            // 逻辑头 == 物理尾，靠 prev 一步拿到前驱 —— 双向链表的价值就在这
            Node *p = tail_;
            tail_ = p->prev;
            if (tail_) tail_->next = nullptr;
            else       head_ = nullptr;
            delete p;
        }
        --size_;
        return true;
    }

    bool empty() const { return size_ == 0; }

    std::string toString() const {
        std::string out;
        out.reserve((size_t)size_ * 4 + 2);
        out.push_back('[');
        if (!rev_) {
            for (Node *p = head_; p; p = p->next) {
                appendNum(out, p->val, p->next != nullptr);
            }
        } else {
            for (Node *p = tail_; p; p = p->prev) {
                appendNum(out, p->val, p->prev != nullptr);
            }
        }
        out.push_back(']');
        return out;
    }
};

// ----------------------------------------------------------------------------
//  把 ADT 串起来跑一组数据：返回该组的输出行（正常输出表，或 "error"）
// ----------------------------------------------------------------------------
static std::string runCase(LinearList &lst,
                           const std::vector<int> &init,
                           const std::string &S) {
    lst.build(init);
    for (size_t i = 0; i < S.size(); i++) {
        char c = S[i];
        if (c == 'R') {
            lst.reverse();
        } else if (c == 'D') {
            if (!lst.removeFirst()) {
                return std::string("error");   // 空表上做 D，本组直接失败
            }
        }
    }
    return lst.toString();
}

// 解析形如 "[12,3,456]" 的串，把其中的整数依次放入 out
static void parseList(const std::string &line, std::vector<int> &out) {
    out.clear();
    int  cur = 0;
    bool has = false;
    for (size_t i = 0; i < line.size(); i++) {
        char c = line[i];
        if (c >= '0' && c <= '9') {
            cur = cur * 10 + (c - '0');
            has = true;
        } else if (has) {
            out.push_back(cur);
            cur = 0;
            has = false;
        }
    }
    if (has) {
        out.push_back(cur);
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int T;
    if (!(std::cin >> T)) {
        return 0;
    }

    std::string      S, line, tok;
    std::vector<int> init;
    std::string      out;
    out.reserve(1 << 13);

    for (int t = 0; t < T; t++) {
        std::cin >> S;                     // 操作串（不含空格）
        int n;
        std::cin >> n;

        // 读表：一直读到出现 ']' 为止，容忍 "[1, 2, 3]" 这种带空格的写法
        line.clear();
        while (line.find(']') == std::string::npos && (std::cin >> tok)) {
            line += tok;
        }

        parseList(line, init);
        if ((int)init.size() > n) {
            init.resize((size_t)n);        // 以 n 为准，保险
        }

        DLinkedList lst;                   // ← 换实现只要改这一行
        out += runCase(lst, init, S);
        out.push_back('\n');
    }

    std::fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
