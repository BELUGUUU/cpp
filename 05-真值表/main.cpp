/*
 * 湖南科技大学 数据结构与算法课程设计   Problem J: 真值表(Ⅲ)
 * 题号：2259        语言：C++
 *
 * 三个阶段的合并实现：
 *   阶段一（词法分析）—— 把逻辑表达式切分为词(token)，非法残缺符号立即定位报错；
 *   阶段二（求值列）  —— 递归下降建立语法树，按变量取值组合逐行求值；
 *   阶段三（错误检测）—— 词法错误优先于语法错误，报出第一个错误所在位置。
 *
 * 关键设计：
 *   1. 词法分析与语法分析严格分成两阶段：先整体切词，任一位置出现词法错误就地抛出；
 *      全部词法合法后再进入语法分析——这样"词法错误优先于语法错误"自然成立。
 *   2. 位置语义：读到末尾仍缺词（含括号未闭合）→ 长度+1；多余/非法词 → 该词 1 起始位置。
 *   3. 优先级由低到高：<-> → || ^ ! （括号最高），每层一个函数，左结合。
 *
 * 编译：g++ -O2 -std=c++11 -o main.exe main.cpp
 */
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/* ======================= 词法单元与错误 ======================= */

struct Token {
    string text;   // 词的正文
    int    pos;    // 1 起始的字符位置
};

// 词法/语法错误：pos 为 1 起始的字符位置
struct ParseErr {
    int pos;
    ParseErr(int p) : pos(p) {}
};

/* ======================= 抽象语法树 ======================= */

// 运算符编码：'<'=等值  'i'=蕴涵  'o'=析取  '&'=合取
struct Node {
    char  op;        // '\0' 表示变量或否定节点
    int   var;       // 变量：字母下标 0..25；非变量为 -1
    bool  isNot;     // 是否否定节点
    Node* l;
    Node* r;
    Node(int v) : op('\0'), var(v), isNot(false), l(NULL), r(NULL) {}          // 变量
    Node(Node* c) : op('\0'), var(-1), isNot(true), l(c), r(NULL) {}           // 否定
    Node(char o, Node* a, Node* b) : op(o), var(-1), isNot(false), l(a), r(b) {} // 二元
};

vector<int> g_val(26, 0);   // 各变量的当前取值

int eval(const Node* nd) {
    if (nd->var >= 0)           return g_val[nd->var];
    if (nd->isNot)              return 1 - eval(nd->l);
    int a = eval(nd->l), b = eval(nd->r);
    switch (nd->op) {
        case '<': return a == b ? 1 : 0;                 // 等值
        case 'i': return (a == 1 && b == 0) ? 0 : 1;     // 蕴涵
        case 'o': return a | b;                          // 析取
        default:  return a & b;                          // 合取
    }
}

void collect(const Node* nd, bool* seen) {
    if (nd->var >= 0) { seen[nd->var] = true; return; }
    if (nd->isNot)    { collect(nd->l, seen); return; }
    collect(nd->l, seen);
    collect(nd->r, seen);
}

/* ======================= 全局解析状态 ======================= */

string        g_s;        // trim 后的表达式
int           g_n;        // 其长度
vector<Token> g_tok;      // 词序列
int           g_pi;       // 解析进度

bool   has()     { return g_pi < (int)g_tok.size(); }
string peek()    { return g_tok[g_pi].text; }
int    peekPos() { return g_tok[g_pi].pos; }
void   next()    { g_pi++; }

/* ======================= 阶段一：词法分析 ======================= */
// 返回词序列；遇到非法字符或残缺的 "<-" "->" "|" 立即抛 ParseErr
void tokenize() {
    g_tok.clear();
    int i = 0;
    while (i < g_n) {
        char c = g_s[i];
        if (c == ' ') { i++; continue; }           // 空格是分隔符，不是词
        int start = i;
        string t;
        if (c == '<') {
            if (i + 2 < g_n && g_s[i + 1] == '-' && g_s[i + 2] == '>') { t = "<->"; i += 3; }
            else throw ParseErr(start + 1);
        } else if (c == '-') {
            if (i + 1 < g_n && g_s[i + 1] == '>') { t = "->"; i += 2; }
            else throw ParseErr(start + 1);
        } else if (c == '|') {
            if (i + 1 < g_n && g_s[i + 1] == '|') { t = "||"; i += 2; }
            else throw ParseErr(start + 1);
        } else if (c == '!' || c == '^' || c == '(' || c == ')' || (c >= 'a' && c <= 'z')) {
            t = string(1, c); i++;
        } else {
            throw ParseErr(start + 1);             // 一切非法字符
        }
        Token tk; tk.text = t; tk.pos = start + 1;
        g_tok.push_back(tk);
    }
}

/* ======================= 阶段二：递归下降语法分析 ======================= */
// 优先级从低到高：<->  ->  ||  ^  !  括号

Node* parseEquiv();
Node* parseImp();
Node* parseOr();
Node* parseAnd();
Node* parseNot();
Node* parseAtom();

Node* parseEquiv() {
    Node* l = parseImp();
    while (has() && peek() == "<->") { next(); l = new Node('<', l, parseImp()); }
    return l;
}
Node* parseImp() {
    Node* l = parseOr();
    while (has() && peek() == "->") { next(); l = new Node('i', l, parseOr()); }
    return l;
}
Node* parseOr() {
    Node* l = parseAnd();
    while (has() && peek() == "||") { next(); l = new Node('o', l, parseAnd()); }
    return l;
}
Node* parseAnd() {
    Node* l = parseNot();
    while (has() && peek() == "^") { next(); l = new Node('&', l, parseNot()); }
    return l;
}
Node* parseNot() {
    if (has() && peek() == "!") { next(); return new Node(parseNot()); }
    return parseAtom();
}
Node* parseAtom() {
    if (!has()) throw ParseErr(g_n + 1);           // 该有词的地方已经到末尾
    string t = peek();
    int p = peekPos();
    next();
    if (t == "(") {                                // 括号
        Node* v = parseEquiv();
        if (!has()) throw ParseErr(g_n + 1);       // 括号没有闭合
        if (peek() != ")") throw ParseErr(peekPos());
        next();
        return v;
    }
    if (t == ")" || t == "!" || t == "^" || t == "||" || t == "->" || t == "<->")
        throw ParseErr(p);                         // 运算符出现在操作数位置
    return new Node(t[0] - 'a');                   // 单个小写字母是变量
}

/* ======================= 主程序 ======================= */

int main() {
    string line;
    string out;
    // 逐行处理，兼容 Windows 的 \r\n 行尾

    while (getline(cin, line)) {
        // 去掉行尾的 \r
        while (!line.empty() && (line[line.size() - 1] == '\r' || line[line.size() - 1] == '\n'))
            line.erase(line.size() - 1);
        // trim 两端空格
        size_t b = line.find_first_not_of(' ');
        size_t e = line.find_last_not_of(' ');
        g_s = (b == string::npos) ? string() : line.substr(b, e - b + 1);
        g_n = (int)g_s.size();

        try {
            tokenize();                       // 阶段一：词法分析（错误优先）
            g_pi = 0;
            Node* root = parseEquiv();        // 阶段二：语法分析
            if (has()) throw ParseErr(peekPos());   // 表达式之后还有多余内容

            bool seen[26];
            for (int i = 0; i < 26; i++) seen[i] = false;
            collect(root, seen);
            vector<int> order;                // 按字母序的变量表
            for (int i = 0; i < 26; i++) if (seen[i]) order.push_back(i);

            // 表头：变量名 + 原表达式
            for (size_t j = 0; j < order.size(); j++) {
                out += (char)('a' + order[j]);
                out += ' ';
            }
            out += g_s;
            out += '\n';

            // 变量取值组合：从全 1 到全 0（每个变量一列，字母序）
            int m = (int)order.size();
            for (int mask = (1 << m) - 1; mask >= 0; mask--) {
                for (int j = 0; j < m; j++) {
                    int bit = (mask >> (m - 1 - j)) & 1;
                    g_val[order[j]] = bit;
                    out += char('0' + bit);
                    out += ' ';
                }
                out += char('0' + eval(root));
                out += '\n';
            }
        } catch (ParseErr& err) {
            out += g_s;
            out += ": Unexpected token at ";
            out += to_string(err.pos);
            out += '\n';
        }
    }
    cout << out;
    return 0;
}
