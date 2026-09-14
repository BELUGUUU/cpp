// Problem C: Josephus问题(I) 链表实现  OJ 1719
// 不带头结点循环链表模拟报数，报到 k 的结点删除
#include <iostream>
using namespace std;

typedef struct LNode {
    int data;
    struct LNode *next;
} LNode, *LinkList;

// 建立 1..n 的循环链表，返回指向 1 号结点的指针
LinkList CreateList(int n) {
    LinkList h = new LNode;
    h->data = 1;
    LinkList tail = h;
    for (int i = 2; i <= n; i++) {
        tail->next = new LNode;
        tail->next->data = i;
        tail = tail->next;
    }
    tail->next = h;
    return h;
}

// 从 h 所指结点开始报 1，删除报到 k 的结点，返回下一轮报数起点
LinkList Execute(LinkList h, int k) {
    LinkList prev = h;
    while (prev->next != h) prev = prev->next;   // 先找到前驱
    for (int i = 1; i < k; i++) { prev = h; h = h->next; }
    prev->next = h->next;
    LinkList nx = h->next;
    delete h;
    return nx;
}

int main() {
    int n, k;
    while (cin >> n >> k) {
        LinkList h;
        h = CreateList(n);
        while (--n)
            h = Execute(h, k);
        cout << h->data << endl;
        delete h;
    }
    return 0;
}
