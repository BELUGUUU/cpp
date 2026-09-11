// ============================================================================
//  线性表 R / D 操作  ——  版本二：顺序表（数组）实现
// ----------------------------------------------------------------------------
//  与版本一共用同一个 ADT 接口 LinearList，只换存储结构，对照两版差异。
//
//  ★ 顺序表怎么做 O(1) 的 D
//      顺序表的"删除第一个元素"按教科书是移位覆盖，O(n)。如果表长 1e5、
//      操作串 1e5，全删一遍就是 O(n^2) = 1e10，超时。
//      所以这里不搬元素，而是维护一个下标窗口 [lo, hi] 表示"当前表在数组
//      中的有效区间"：
//          build : lo = 0, hi = n-1
//          D     : rev 为假 -> ++lo；rev 为真 -> --hi          O(1)
//          empty : lo > hi
//      配合 rev 标志，R / D 全是 O(1)，扫描完 S 只需 O(|S|)。
//      （边界回收：删掉的元素原地不动，只在最后输出 [lo,hi] 区间。
//        lo/hi 单调收缩，最多各走 n 步，总位移量 O(n)。）
//
//  ★ 与双向链表的取舍
//      顺序表：内存连续、常数小、随机访问快；但中间插入/删除要搬元素，
//              容量固定或需扩容，删除靠"窗口/懒惰删除"绕开搬运。
//      双链表：任意位置插入删除都是 O(1)、内存按需分配；但每个结点多两个
//              指针，缓存不友好，常数更大。
//      本题两种都达到 O(|S| + n)，差别只在实际运行常数上（见实验数据）。
//
//  复杂度
//      建表 O(n)（一次拷贝）；扫描操作串 O(|S|)；输出 O(n)；空间 O(n)。
// ============================================================================

#include <cstdio>
#include <string>
#include <vector>
#include <iostream>

// ============================================================================
//  ADT LinearList  ——  与版本一完全相同的接口定义
// ============================================================================
//  数据对象 : D = { a_i | a_i ∈ [1,100], i = 1..n, n >= 0 }
//  数据关系 : R = { <a_(i-1), a_i> }
//  基本操作 : build / reverse / removeFirst / empty / toString
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
//  实现二：顺序表（连续数组 + 下标窗口 + 逆转标志）
// ----------------------------------------------------------------------------
class ArrayList : public LinearList {
private:
    std::vector<int> a_;      // 顺序存储的数组
    int  lo_, hi_;            // 当前表占据 a_[lo_ .. hi_]（含两端）
    bool rev_;                // 逻辑顺序是否与物理顺序相反

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
    ArrayList() : lo_(0), hi_(-1), rev_(false) {}

    void build(const std::vector<int> &a) {
        a_ = a;                       // 一次整块拷贝
        lo_ = 0;
        hi_ = (int)a_.size() - 1;
        rev_ = false;
    }

    void reverse() { rev_ = !rev_; }   // 只翻标志，O(1)

    bool removeFirst() {
        if (empty()) {
            return false;              // 空表删除 -> error
        }
        if (!rev_) {
            ++lo_;                     // 逻辑头在左边，收缩左端
        } else {
            --hi_;                     // 逻辑头在右边，收缩右端
        }
        return true;
    }

    bool empty() const { return lo_ > hi_; }

    std::string toString() const {
        std::string out;
        if (empty()) {
            return std::string("[]");
        }
        out.reserve((size_t)(hi_ - lo_ + 1) * 4 + 2);
        out.push_back('[');
        if (!rev_) {
            for (int i = lo_; i <= hi_; i++) {
                appendNum(out, a_[(size_t)i], i < hi_);
            }
        } else {
            for (int i = hi_; i >= lo_; i--) {
                appendNum(out, a_[(size_t)i], i > lo_);
            }
        }
        out.push_back(']');
        return out;
    }
};

// ----------------------------------------------------------------------------
//  驱动逻辑与版本一相同，只是换成了顺序表实现
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
                return std::string("error");
            }
        }
    }
    return lst.toString();
}

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
        std::cin >> S;
        int n;
        std::cin >> n;

        line.clear();
        while (line.find(']') == std::string::npos && (std::cin >> tok)) {
            line += tok;
        }

        parseList(line, init);
        if ((int)init.size() > n) {
            init.resize((size_t)n);
        }

        ArrayList lst;                     // ← 与版本一唯一的差别
        out += runCase(lst, init, S);
        out.push_back('\n');
    }

    std::fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
