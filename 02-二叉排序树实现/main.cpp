/**
 * 问题 K: 单词检查(Ⅱ) - 二叉排序树实现（C++）
 *
 * 一、数据结构
 *   二叉排序树（BST）维护字典：按 strcmp 字典序插入，左小右大。
 *   结点只存单词在 pool[] 中的下标 idx，不存第二份字符串：
 *   该下标同时就是单词在字典文件中的出现次序，用于还原输出的先后顺序。
 *
 * 二、输出要求
 *   第一行：BST 的后序遍历（每个单词后跟一个空格）；之后逐个检查待测单词。
 *
 * 三、建议词生成（题目规定的三种操作）
 *   (1) 在每一个可能位置插入 'a'-'z' 中的一个  -> 长度 +1
 *   (2) 删除单词中的一个字符                   -> 长度 -1
 *   (3) 用 'a'-'z' 中的一个取代任一字符         -> 长度不变
 *   生成候选后到 BST 中检索，命中即为建议词；命中结果按 idx 升序排序即得原始次序。
 *
 * 四、两个"保命"细节
 *   1) 插入与后序遍历都用【迭代】实现（后序用双栈法）。
 *      若字典按字典序给出，BST 会退化成一条 n 层的链，递归版本极易栈溢出。
 *   2) 后序遍历输出每个词后都跟一个空格（题目明确要求），最后一个也不例外。
 *
 * 五、复杂度
 *   建树 O(n·L·log n)（平均）；后序 O(n)；
 *   单次查询约 26(L+1)+25L+L ≈ 53L 个候选，每个在树上走 O(L·log n) -> O(L²·log n) 量级；
 *   总体 O(n·L·log n + m·L²·log n)。
 *   与顺序表 O(m·n·L) 相比，优势随字典规模增大而拉开；
 *   但 BST 最坏（有序输入）退化为 O(n)，此时与顺序表同级。
 */
#include <cstdio>
#include <cstring>
#include <cstdlib>
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
//                        二叉排序树
// ======================================================================

/** 按原始次序存放的单词池：pool[i] 的字典原始次序就是 i */
static vector<string> pool;

struct Node {
    int   idx;              // pool 下标 = 字典原始次序
    Node *left;
    Node *right;

    explicit Node(int i) : idx(i), left(nullptr), right(nullptr) {}
};

static Node *root = nullptr;
static int   nodeCount = 0;

/** 迭代插入，避免字典有序（退化成单链）时递归爆栈 */
static void insertNode(int idx) {
    Node *node = new Node(idx);
    nodeCount++;
    if (root == nullptr) {
        root = node;
        return;
    }
    Node *cur = root;
    while (true) {
        int cmp = strcmp(pool[idx].c_str(), pool[cur->idx].c_str());
        if (cmp < 0) {
            if (cur->left == nullptr) {
                cur->left = node;
                return;
            }
            cur = cur->left;
        } else if (cmp > 0) {
            if (cur->right == nullptr) {
                cur->right = node;
                return;
            }
            cur = cur->right;
        } else {
            delete node;                    // 题目保证单词互不相同，仅作防御
            nodeCount--;
            return;
        }
    }
}

/** 迭代检索：命中返回结点下标（= 原始次序），未命中返回 -1 */
static int searchNode(const string &w) {
    Node *cur = root;
    const char *key = w.c_str();
    while (cur != nullptr) {
        int cmp = strcmp(key, pool[cur->idx].c_str());
        if (cmp == 0) {
            return cur->idx;
        }
        cur = (cmp < 0) ? cur->left : cur->right;
    }
    return -1;
}

/**
 * 迭代式后序遍历（双栈法）：
 * 按"根-右-左"的顺序压入 result 栈，再依次弹出即得"左-右-根"。
 */
static void collectPostOrder(vector<int> &out) {
    out.clear();
    if (root == nullptr) {
        return;
    }
    vector<Node *> task;
    vector<Node *> result;
    task.reserve((size_t) nodeCount);
    result.reserve((size_t) nodeCount);
    task.push_back(root);
    while (!task.empty()) {
        Node *cur = task.back();
        task.pop_back();
        result.push_back(cur);
        if (cur->left != nullptr) {
            task.push_back(cur->left);
        }
        if (cur->right != nullptr) {
            task.push_back(cur->right);
        }
    }
    for (int i = (int) result.size() - 1; i >= 0; i--) {
        out.push_back(result[i]->idx);
    }
}

// ======================================================================
//                          建议词生成
// ======================================================================

/**
 * 生成候选词并检索 BST，命中则把其字典次序写入 hits，返回命中个数。
 *
 * 候选词去重（不依赖任何集合容器）：
 *   - 删除：删 i 与删 i-1 结果相同 <=> word[i] == word[i-1]，故 cs[i]==cs[i-1] 时跳过；
 *   - 插入：在 i 处插入 c 与在 i+1 处插入 c 结果相同 <=> word[i] == c，故 cs[i]==c 时跳过；
 *   - 替换：跳过"换成自己"。
 *   三条合起来保证每个候选串只生成一次，命中的字典词不会重复输出。
 */
static int findSuggestions(const string &word, int *hits) {
    int len = (int) word.size();
    int cnt = 0;

    char cs[MAX_WORD_LEN + 1];
    for (int i = 0; i < len && i <= MAX_WORD_LEN; i++) {   // 带上界，写入恒不越界
        cs[i] = word[i];
    }

    // ---------- (2) 删除一个字符 ----------
    for (int i = 0; i < len; i++) {
        if (i > 0 && cs[i] == cs[i - 1]) {
            continue;
        }
        string cand;
        cand.reserve((size_t) len - 1);
        cand.append(word, 0, (size_t) i);
        cand.append(word, (size_t) i + 1, (size_t) (len - i - 1));
        int idx = searchNode(cand);
        if (idx >= 0) {
            hits[cnt++] = idx;
        }
    }

    // ---------- (3) 替换一个字符 ----------
    for (int i = 0; i < len; i++) {
        char origin = cs[i];
        for (char c = 'a'; c <= 'z'; c++) {
            if (c == origin) {
                continue;
            }
            cs[i] = c;
            int idx = searchNode(string(cs, (size_t) len));
            if (idx >= 0) {
                hits[cnt++] = idx;
            }
        }
        cs[i] = origin;
    }

    // ---------- (1) 在每一个可能位置插入一个字符 ----------
    if (len < MAX_WORD_LEN) {
        for (int i = 0; i <= len; i++) {
            for (char c = 'a'; c <= 'z'; c++) {
                if (i < len && cs[i] == c) {
                    continue;               // 插到相同字符前面 == 插到它后面，去重
                }
                string cand;
                cand.reserve((size_t) len + 1);
                cand.assign(cs, (size_t) i);
                cand.push_back(c);
                cand.append(cs + i, (size_t) (len - i));
                int idx = searchNode(cand);
                if (idx >= 0) {
                    hits[cnt++] = idx;
                }
            }
        }
    }

    return cnt;
}

/** 按字典原始次序做插入排序（命中数最多几百个，插入排序足够） */
static void sortByOrder(int *a, int len) {
    for (int i = 1; i < len; i++) {
        int key = a[i];
        int j = i - 1;
        while (j >= 0 && a[j] > key) {
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = key;
    }
}

/** 迭代释放整棵树：同样是递归深度问题，退化成链时递归释放会爆栈 */
static void destroyTree() {
    if (root == nullptr) {
        return;
    }
    vector<Node *> stack;
    stack.push_back(root);
    while (!stack.empty()) {
        Node *cur = stack.back();
        stack.pop_back();
        if (cur->left != nullptr) {
            stack.push_back(cur->left);
        }
        if (cur->right != nullptr) {
            stack.push_back(cur->right);
        }
        delete cur;
    }
    root = nullptr;
    nodeCount = 0;
}

// ======================================================================
//                              main
// ======================================================================

int main() {
    FastReader in(stdin);

    // ---------- 1. 读字典并建树 ----------
    pool.reserve(1 << 16);
    string w;
    while (in.next(w)) {
        if (w == "#") {
            break;
        }
        pool.push_back(w);
    }
    for (int i = 0; i < (int) pool.size(); i++) {
        insertNode(i);
    }

    // ---------- 2. 第一行：后序遍历 ----------
    string out;
    out.reserve(1 << 20);
    vector<int> postOrder;
    collectPostOrder(postOrder);
    for (int i = 0; i < (int) postOrder.size(); i++) {
        out += pool[postOrder[i]];
        out += ' ';
    }
    out += '\n';

    // ---------- 3. 逐个检查待测单词 ----------
    vector<int> hits(4096);
    while (in.next(w)) {
        if (w == "#") {
            break;
        }
        if (searchNode(w) >= 0) {
            out += w;
            out += " is correct\n";
            continue;
        }
        int cnt = findSuggestions(w, &hits[0]);
        sortByOrder(&hits[0], cnt);
        out += w;
        out += ':';
        for (int i = 0; i < cnt; i++) {
            out += ' ';
            out += pool[hits[i]];
        }
        out += '\n';
    }

    fwrite(out.data(), 1, out.size(), stdout);
    destroyTree();
    return 0;
}
