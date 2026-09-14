// Problem P: 二叉树的创建和文本显示  OJ 1826
// 先序串建树（# 为空），输出树形左旋 90 度：右子树在上、根居中、左子树在下，每层缩进 4
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

void print(Node *n, int depth) {
    if (!n) return;
    print(n->r, depth + 1);              // 右子树在上方
    for (int i = 0; i < depth * 4; i++) cout << ' ';
    cout << n->data << "\n";             // 行尾无空格
    print(n->l, depth + 1);              // 左子树在下方
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
        print(root, 0);
        cout << "\n";                    // 每组数据后输出一空行
        freeTree(root);
    }
    return 0;
}
