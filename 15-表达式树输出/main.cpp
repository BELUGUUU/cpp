// Problem Q: 表达式树的创建与输出  OJ 1827
// 先序串建表达式树，输出全括号表达式：叶子直接输出，内部结点 (左 op 右)
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
    if (!n->l && !n->r) return n->data;  // 叶子：数字直接输出
    return "(" + expr(n->l) + n->data + expr(n->r) + ")";
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
        if (root) cout << expr(root) << "\n";
        freeTree(root);
    }
    return 0;
}
