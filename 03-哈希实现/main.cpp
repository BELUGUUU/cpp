/**
 * 问题 K: 单词检查(Ⅲ) - 哈希表实现（C++）
 *
 * ============================ 设计说明 ============================
 * 一、关于"不能采用泛型库"
 *   本程序没有使用任何现成的哈希容器（unordered_map / unordered_set / map / set），
 *   哈希函数、哈希表、冲突处理全部手写。仅使用 string / vector 作为字符串与
 *   可增长数组的载体（属于基础类型，不是哈希容器）。
 *
 * 二、哈希函数（三个可互换，均可对比）
 *   hashELF        : 题目给出的 UNIX ELF hash，成熟算法，作为基准
 *   hashPolynomial : 自行设计的多项式哈希（BKDR 风格，乘数 131）
 *   hashNaive      : 反面教材，字符直接累加，低位分布极差，仅用于对比实验
 *
 *   注意：题目给出的 ELF hash 原型是 unsigned long。在 64 位平台上 unsigned long
 *   是 64 位，与算法"用高 4 位回灌"的设计意图不符，因此这里统一用 uint32_t。
 *
 * 三、冲突处理（两种都实现，可通过 STRATEGY 切换）
 *   1) 链地址法   ChainHashDict  : 每个槽挂一条链表（用 int 数组当指针，不用 new 结点）
 *   2) 线性地址法 ProbeHashDict  : 开放定址，冲突后线性探测下一个空槽
 *   表容量统一取 2 的幂，用 (hash & mask) 代替 (hash % cap)，位运算更快。
 *
 * 四、省内存的关键设计
 *   字典读进 pool[]（按原始出现次序保存），哈希表内部只存"单词在 pool 中的下标"，
 *   不再存第二份字符串。这个下标同时就是该词的原始次序，一举两得：
 *     - 50 万词时省下约 16MB 的重复字符串；
 *     - 题目要求建议词"按字典出现先后次序"输出，而哈希表完全不保序，
 *       命中结果按这个下标升序排一下即可还原顺序。
 *
 * 五、复杂度
 *   建表 O(n)（期望）；单次查找 O(1+α)（期望），α 为装填因子；
 *   成功查找的平均比较次数：链地址法约 1+α/2，线性探测法约 (1+1/(1-α))/2。
 *   最坏情况（哈希全冲突）两者都退化为 O(n)。
 *
 * 六、候选词去重（不依赖任何集合容器，只靠下标判断）
 *   三类候选词长度互不相同（L-1 / L / L+1），故跨类不可能重复；同类内部：
 *     - 替换：跳过"换成自己"，不同 (位置, 字符) 组合得到的串必不相同；
 *     - 删除：删 i 与删 i-1 结果相同 <=> word[i] == word[i-1]，
 *             故 cs[i] == cs[i-1] 时跳过，每段连续相同字符只删第一次；
 *     - 插入：在 i 处插入 c 与在 i+1 处插入 c 结果相同 <=> word[i] == c，
 *             故 i < len && cs[i] == c 时跳过。
 *   三条合起来保证每个候选串只生成一次，因此命中的字典词不会重复输出。
 *
 * 附注（编译告警说明）：
 *   若用 g++ 16 且打开 -Wall，可能会看到针对 ChainHashDict::indexOf 的
 *   "-Warray-bounds" 告警。那是 GCC 推测性去虚化（speculative devirtualization）
 *   在常量折叠出 ProbeHashDict 后仍内联了另一个候选目标造成的误报：
 *   加 -fno-devirtualize 后告警完全消失，且程序输出始终正确。
 *   提交 OJ 时按惯例不加 -Wall，不会有任何告警。
 */
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <string>
#include <vector>

using std::string;
using std::vector;

// ------------------------------ 可调参数 ------------------------------

static const int   MAX_WORD_LEN    = 15;    // 题目规定单词最大长度
static const int   MAX_SUGGESTIONS = 4096;  // 候选上限约 800，留足余量

static const int   CHAINING = 0;
static const int   PROBING  = 1;
static const int   STRATEGY = PROBING;      // ← 提交时使用哪种冲突处理
static const float LOAD_FACTOR = 0.5f;      // 装填因子

typedef uint32_t u32;
typedef u32 (*HashFunc)(const char *s, int len);

// ======================================================================
//                              哈希函数
// ======================================================================

/**
 * 自行设计：多项式哈希，乘数取质数 131。
 * 乘数足够大且为质数，能让高位变化影响到低位，显著减少冲突。
 */
static inline u32 hashPolynomial(const char *s, int len) {
    u32 h = 0;
    for (int i = 0; i < len; i++) {
        h = h * 131u + (unsigned char) s[i];   // uint32 自然溢出，等价于 mod 2^32
    }
    return h;
}

/**
 * UNIX ELF hash（题目给出的成熟算法）。
 * 每读一个字符左移 4 位累加；若高 4 位非零，则把它异或回低位并清掉，
 * 使字符信息扩散到整个 32 位。
 */
static inline u32 hashELF(const char *s, int len) {
    u32 h = 0;
    for (int i = 0; i < len; i++) {
        h = (h << 4) + (unsigned char) s[i];
        u32 g = h & 0xF0000000u;
        if (g != 0) {
            h ^= (g >> 24);
            h &= ~g;
        }
    }
    return h;
}

/** 反面教材：字符值累加。低位几乎不变，冲突极其严重，仅用于对比实验。 */
static inline u32 hashNaive(const char *s, int len) {
    u32 h = 0;
    for (int i = 0; i < len; i++) {
        h += (unsigned char) s[i];
    }
    return h;
}

// ======================================================================
//                              字典抽象
// ======================================================================

/** 哈希表内部只存"单词在 pool 中的下标"（该下标同时就是字典原始次序） */
struct Dict {
    const vector<string> *pool;   // 单词池，按字典原始次序存放
    long long probes;             // 累计比较次数，用于定量评估冲突情况

    Dict(const vector<string> *p) : pool(p), probes(0) {}
    virtual ~Dict() {}

    virtual void put(int order) = 0;                 // 插入 pool[order]
    virtual int  indexOf(const string &w) = 0;       // 命中返回原始次序，否则 -1
    virtual int  capacity() const = 0;               // 槽位数（用于对比表大小影响）
};

/** 由词数 + 装填因子反推 2 的幂容量 */
static int tableSizeFor(int expected, float loadFactor) {
    if (expected < 1) expected = 1;
    int need = (int) (expected / (double) loadFactor) + 1;
    int cap = 4;
    while (cap < need && cap < (1 << 30)) {
        cap <<= 1;
    }
    return cap;
}

// ======================================================================
//                     实现一：链地址法（拉链法）
// ======================================================================

/**
 * 每个槽挂一条链表。链表不用 new 出来的结点，而是三个平行数组：
 *   head[slot] -> 该槽第一个结点下标（-1 表示空）
 *   nxt[node]  -> 同槽下一个结点
 *   ord[node]  -> 该结点对应的单词在 pool 中的下标（= 字典原始次序）
 * 优点：负载因子可以高、删除方便；缺点：链表指针跳转，缓存不友好。
 */
struct ChainHashDict : Dict {
    HashFunc hf;
    float    lf;
    vector<int> head;
    vector<int> nxt;
    vector<int> ord;
    u32 mask;
    int sz;

    ChainHashDict(const vector<string> *p, int expected, float loadFactor, HashFunc f)
        : Dict(p), hf(f), lf(loadFactor), sz(0) {
        int cap = tableSizeFor(expected, loadFactor);
        head.assign(cap, -1);
        mask = (u32) (cap - 1);
        int n = expected > 0 ? expected : 1;
        nxt.reserve(n);
        ord.reserve(n);
    }

    void put(int order) {
        if ((double) (sz + 1) / (double) head.size() > lf) {
            rehash();
        }
        const string &w = (*pool)[order];
        u32 slot = hf(w.data(), (int) w.size()) & mask;
        int node = sz++;
        nxt.push_back(head[slot]);      // 头插法
        ord.push_back(order);
        head[slot] = node;
    }

    void rehash() {
        int newCap = (int) head.size() << 1;
        head.assign(newCap, -1);
        mask = (u32) (newCap - 1);
        for (int p = 0; p < sz; p++) {
            const string &w = (*pool)[ord[p]];
            u32 slot = hf(w.data(), (int) w.size()) & mask;
            nxt[p] = head[slot];
            head[slot] = p;
        }
    }

    int indexOf(const string &w) {
        u32 slot = hf(w.data(), (int) w.size()) & mask;
        for (int p = head[slot]; p != -1; p = nxt[p]) {
            probes++;
            if ((*pool)[ord[p]] == w) {
                return ord[p];
            }
        }
        return -1;
    }

    int capacity() const { return (int) head.size(); }
};

// ======================================================================
//                  实现二：线性地址法（开放定址）
// ======================================================================

/**
 * 冲突时依次探测 slot+1, slot+2 ... 直到找到空槽。
 * 表里直接存 pool 下标，-1 表示空槽（这样不必为每个空槽保存一个空字符串）。
 * 优点：无指针、访存连续、缓存命中率高；
 * 缺点：容易产生"一次聚集"，装填因子必须留余量，且删除麻烦（本题只插不删）。
 */
struct ProbeHashDict : Dict {
    HashFunc hf;
    float    lf;
    vector<int> table;      // -1 = 空槽，否则为 pool 下标
    u32 mask;
    int sz;

    ProbeHashDict(const vector<string> *p, int expected, float loadFactor, HashFunc f)
        : Dict(p), hf(f), lf(loadFactor), sz(0) {
        int cap = tableSizeFor(expected, loadFactor);
        table.assign(cap, -1);
        mask = (u32) (cap - 1);
    }

    void put(int order) {
        if ((double) (sz + 1) / (double) table.size() > lf) {
            rehash();
        }
        insertRaw(order);
    }

    void insertRaw(int order) {
        const string &w = (*pool)[order];
        u32 slot = hf(w.data(), (int) w.size()) & mask;
        while (table[slot] != -1) {
            slot = (slot + 1) & mask;
        }
        table[slot] = order;
        sz++;
    }

    void rehash() {
        int newCap = (int) table.size() << 1;
        vector<int> old;
        old.swap(table);
        table.assign(newCap, -1);
        mask = (u32) (newCap - 1);
        sz = 0;
        for (size_t i = 0; i < old.size(); i++) {
            if (old[i] != -1) {
                insertRaw(old[i]);
            }
        }
    }

    int indexOf(const string &w) {
        u32 slot = hf(w.data(), (int) w.size()) & mask;
        while (table[slot] != -1) {
            probes++;
            if ((*pool)[table[slot]] == w) {
                return table[slot];
            }
            slot = (slot + 1) & mask;
        }
        return -1;
    }

    int capacity() const { return (int) table.size(); }
};

// ======================================================================
//                              主流程
// ======================================================================

static Dict *createDict(int strategy, const vector<string> *pool,
                        int expected, float loadFactor, HashFunc hf) {
    if (strategy == PROBING) {
        return new ProbeHashDict(pool, expected, loadFactor, hf);
    }
    return new ChainHashDict(pool, expected, loadFactor, hf);
}

/**
 * 生成候选词并查表，命中则把其字典次序写入 hits，返回命中个数。
 *
 * 三种操作（题目规定）：
 *   (1) 在每一个可能位置插入 'a'-'z' 中的一个
 *   (2) 删除单词中的一个字符
 *   (3) 用 'a'-'z' 中的一个取代任一字符
 */
static int collectSuggestions(const string &word, Dict &dict, int *hits) {
    int len = (int) word.size();
    int cnt = 0;

    char cs[MAX_WORD_LEN + 1];
    for (int i = 0; i < len && i <= MAX_WORD_LEN; i++) {   // 带上界，写入恒不越界
        cs[i] = word[i];
    }

    // ---------- (2) 删除一个字符 ----------
    for (int i = 0; i < len; i++) {
        if (i > 0 && cs[i] == cs[i - 1]) {
            continue;                       // 与删前一个字符结果相同，天然去重
        }
        string cand;
        cand.reserve((size_t) len - 1);
        cand.append(word, 0, (size_t) i);
        cand.append(word, (size_t) i + 1, (size_t) (len - i - 1));
        int ord = dict.indexOf(cand);
        if (ord >= 0) {
            hits[cnt++] = ord;
        }
    }

    // ---------- (3) 替换一个字符 ----------
    for (int i = 0; i < len; i++) {
        char origin = cs[i];
        for (char c = 'a'; c <= 'z'; c++) {
            if (c == origin) {
                continue;                   // 换成自己等于没改
            }
            cs[i] = c;
            int ord = dict.indexOf(string(cs, (size_t) len));
            if (ord >= 0) {
                hits[cnt++] = ord;
            }
        }
        cs[i] = origin;
    }

    // ---------- (1) 在每一个可能位置插入一个字符 ----------
    if (len < MAX_WORD_LEN) {               // 插入后长度 16 的词不可能出现在字典里
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
                int ord = dict.indexOf(cand);
                if (ord >= 0) {
                    hits[cnt++] = ord;
                }
            }
        }
    }

    return cnt;
}

/** 按字典原始次序做插入排序。命中数最多几百且通常很小，插入排序足够。 */
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

// ======================================================================
//                              快速输入
// ======================================================================

/**
 * 基于 fread 缓冲的读词器。字典可能多达 50 万行，
 * 逐字符读入并直接切分成单词，避免 scanf/cin 的格式解析开销。
 */
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

    /** 读一个非空白 token（单词与 '#' 都不含空白），文件结束返回 false */
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
//                                main
// ======================================================================

#ifndef BENCH_BUILD      // 定义 BENCH_BUILD 可把本文件作为库引入基准测试程序

int main() {
    FastReader in(stdin);

    // ---------- 1. 读字典：按原始次序存入 pool ----------
    vector<string> pool;
    pool.reserve(1 << 16);
    string w;
    while (in.next(w)) {
        if (w == "#") {
            break;
        }
        pool.push_back(w);
    }
    int n = (int) pool.size();

    // ---------- 2. 建哈希表 ----------
    Dict *dict = createDict(STRATEGY, &pool, n, LOAD_FACTOR, hashELF);
    for (int i = 0; i < n; i++) {
        dict->put(i);
    }

    // ---------- 3. 逐个检查待测单词 ----------
    string out;
    out.reserve(1 << 20);
    vector<int> hits(MAX_SUGGESTIONS);

    while (in.next(w)) {
        if (w == "#") {
            break;
        }
        if (dict->indexOf(w) >= 0) {
            out += w;
            out += " is correct\n";
            continue;
        }
        int cnt = collectSuggestions(w, *dict, &hits[0]);
        sortByOrder(&hits[0], cnt);                 // 还原为字典中的先后次序
        out += w;
        out += ':';
        for (int i = 0; i < cnt; i++) {
            out += ' ';
            out += pool[hits[i]];
        }
        out += '\n';
    }

    fwrite(out.data(), 1, out.size(), stdout);
    delete dict;
    return 0;
}

#endif  // BENCH_BUILD
