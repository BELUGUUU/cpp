/**
 * 问题 K: 单词检查(Ⅰ) - 顺序表实现（C++）
 *
 * 思路：
 *   1. 用数组（顺序表）按输入顺序保存字典，最多 50 万个单词。
 *   2. 对每个待检测单词 w 顺序扫描字典：
 *      - 存在完全相同的词 -> 输出 "w is correct"；
 *      - 否则记录所有与 w 互为"一次编辑"（插入/删除/替换一个字符）的字典词。
 *      由于是从前往后扫描顺序表，天然满足"按字典出现先后次序输出"的要求。
 *
 * 复杂度：预处理 O(n)；单次查询 O(n·L)；m 次查询共 O(m·n·L)。
 *         空间 O(n·L)（字典本身）。
 *   其中 n = 字典词数(≤500000)、m = 待查词数(≤200)、L = 单词最大长度(≤15)。
 *   按最坏规模估算约 200 × 500000 × 15 = 1.5×10^9 次字符比较，
 *   顺序表在本题规模下会明显吃力 —— 这正是引入 BST / 哈希的动机。
 */
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

using std::string;
using std::vector;

static const int MAX_WORD_LEN = 15;

// ======================================================================
//                         快速输入
// ======================================================================

struct FastReader {
    static const int BUFSZ = 1 << 16;
    FILE *f;
    char  buf[BUFSZ];
    int   pos, len;

    explicit FastReader(FILE *fp) : f(fp), pos(0), len(0) {}

    inline int readByte() {
        if (pos >= len) {
            len = (int) fread(buf, 1, BUFSZ, f);
            pos = 0;
            if (len <= 0) {
                return -1;
            }
        }
        return (unsigned char) buf[pos++];
    }

    bool next(string &out) {
        int c = readByte();
        while (c >= 0 && c <= ' ') {
            c = readByte();
        }
        if (c < 0) {
            return false;
        }
        out.clear();
        while (c > ' ') {
            out.push_back((char) c);
            c = readByte();
        }
        return true;
    }
};

// ======================================================================
//                          核心判定
// ======================================================================

/**
 * 判断 a 与 b 是否"恰好差一次编辑"：
 * b 能由 a 经过 (1) 插入一个字符 (2) 删除一个字符 (3) 替换一个字符
 * 中的恰好一次操作得到；完全相同返回 false。
 */
static bool oneEditAway(const string &a, const string &b) {
    int la = (int) a.size();
    int lb = (int) b.size();
    if (la > lb ? la - lb > 1 : lb - la > 1) {
        return false;                       // 长度差超过 1，一次编辑不可能完成
    }

    if (la == lb) {                         // 只可能是"替换"
        int diff = 0;
        for (int i = 0; i < la; i++) {
            if (a[i] != b[i] && ++diff > 1) {
                return false;
            }
        }
        return diff == 1;                   // 必须有且仅有 1 处不同
    }

    // 长度差 1：只可能是"插入"或"删除"，等价于长串比短串多出恰好一个字符
    const string &shorter = (la < lb) ? a : b;
    const string &longer  = (la < lb) ? b : a;
    int i = 0, j = 0, skipped = 0;
    int ls = (int) shorter.size();
    int ll = (int) longer.size();
    while (i < ls && j < ll) {
        if (shorter[i] == longer[j]) {
            i++;
            j++;
        } else {
            if (++skipped > 1) {
                return false;               // 已跳过 2 个字符，不止一次编辑
            }
            j++;                            // 跳过长串中多出的那个字符
        }
    }
    return true;
}

// ======================================================================
//                             main
// ======================================================================

int main() {
    FastReader in(stdin);

    // ---------- 1. 读字典：顺序表（数组）按输入顺序保存 ----------
    vector<string> dict;
    dict.reserve(1 << 16);
    string w;
    while (in.next(w)) {
        if (w == "#") {
            break;
        }
        dict.push_back(w);
    }
    int n = (int) dict.size();

    // ---------- 2. 逐个检查 ----------
    string out;
    out.reserve(1 << 20);
    while (in.next(w)) {
        if (w == "#") {
            break;
        }
        out += w;
        bool correct = false;
        string suggestions;
        for (int i = 0; i < n; i++) {
            if (dict[i] == w) {              // 命中字典 -> 不是错误单词
                correct = true;
                break;
            }
            if (oneEditAway(w, dict[i])) {   // 可由一次插入/删除/替换得到
                suggestions += ' ';
                suggestions += dict[i];
            }
        }
        if (correct) {
            out += " is correct\n";
        } else {
            out += ':';
            out += suggestions;
            out += '\n';
        }
    }

    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
