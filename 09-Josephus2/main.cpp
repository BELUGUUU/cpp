// Problem D: Josephus问题(Ⅱ) 动态数组实现  OJ 2551
// 用动态数组模拟循环报数；删除位置后的元素左移，保留下一轮起点。
// 顺序表删除报到 k 的人，后续元素前移
#include <iostream>
using namespace std;

// 结构体封装动态数组
typedef struct {
    int *data;   // 动态分配的存储数组
    int size;    // 当前剩余人数
} Array;

void CreateArray(Array &a, int n) {
    a.data = new int[n];
    for (int i = 0; i < n; i++) a.data[i] = i + 1;
    a.size = n;
}

// 从下标 index 的人开始报 1，删除报到 k 的人，返回新的报数起点下标
int Execute(Array &a, int index, int k) {
    int out = (index + k - 1) % a.size;   // 本轮报到 k 的下标
    for (int i = out; i < a.size - 1; i++) a.data[i] = a.data[i + 1];
    a.size--;
    return out % a.size;
}

int main() {
    int n, k;
    while (cin >> n >> k) {
        Array a;
        CreateArray(a, n);
        int index = 0;               // 从第 1 个人（下标 0）开始报数
        while (--n)
            index = Execute(a, index, k);

        // 此时 size 为 1，唯一剩下的元素在 data[0]
        cout << a.data[0] << endl;

        // 规范释放动态数组内存
        delete[] a.data;
    }
    return 0;
}
