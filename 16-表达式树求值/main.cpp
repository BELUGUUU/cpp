// Problem R: 表达式树的值  OJ 1828
// 构树后递归求值：叶子转数字，内部结点对左右子树结果执行运算。
// 在 Q 的全括号输出基础上求整数值，输出 (表达式)=值
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

struct Node {
    string data;
    Node *l, *r;
    Node(const string &d) : data(d), l(0), r(0) {}
};

Node *build(istringstream &is) {
    string tok;
    if (!(is >> tok) || tok == "#") return NULL;
    Node *n = new Node(tok);
    n->l = build(is);
    n->r = build(is);
    return n;
}

string expr(Node *n) {
    if (!n->l && !n->r) return n->data;
    return "(" + expr(n->l) + n->data + expr(n->r) + ")";
}

long long evalTree(Node *n) {
    // 后序求值保证运算前，左右操作数已经算出。
    if (!n->l && !n->r) {                        // 叶子：数值
        long long v = 0;
        for (size_t i = 0; i < n->data.size(); i++) v = v * 10 + n->data[i] - '0';
        return v;
    }
    long long a = evalTree(n->l), b = evalTree(n->r);
    char c = n->data[0];
    if (c == '+') return a + b;
    if (c == '-') return a - b;
    if (c == '*') return a * b;
    return a / b;
}

void freeTree(Node *n) {
    if (n) { freeTree(n->l); freeTree(n->r); delete n; }
}

int main() {
    string line;
    while (getline(cin, line)) {
        if (line.find_first_not_of(" \r\t") == string::npos) continue;
        istringstream is(line);
        Node *root = build(is);
        if (root) cout << expr(root) << "=" << evalTree(root) << "\n";
        freeTree(root);
    }
    return 0;
}
