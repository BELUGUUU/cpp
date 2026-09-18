// Problem O: 中缀表达式转后缀表达式  OJ 1823
// 借助运算符栈处理优先级与括号，最终输出可直接计算的后缀表达式。
// 经典调度场算法：操作数直接输出，运算符按优先级出入栈
#include <iostream>
#include <stack>
#include <string>
#include <cctype>
using namespace std;

int prec(char c) {
    // 返回优先级；数字和括号不参与这里的比较。
    if (c == '+' || c == '-') return 1;
    if (c == '*' || c == '/') return 2;
    return 0;
}

int main() {
    string s;
    while (getline(cin, s)) {
        string out;
        stack<char> ops;
        for (size_t i = 0; i < s.size(); i++) {
            char c = s[i];
            if (isspace((unsigned char)c)) continue;
            if (isalnum((unsigned char)c)) {
                out += c;                            // 操作数直接输出
            } else if (c == '(') {
                ops.push(c);
            } else if (c == ')') {
                while (!ops.empty() && ops.top() != '(') { out += ops.top(); ops.pop(); }
                if (!ops.empty()) ops.pop();         // 弹出左括号，不输出
            } else {                                 // + - * /
                while (!ops.empty() && ops.top() != '(' && prec(ops.top()) >= prec(c)) {
                    out += ops.top(); ops.pop();
                }
                ops.push(c);
            }
        }
        while (!ops.empty()) { out += ops.top(); ops.pop(); }
        cout << out << "\n";
    }
    return 0;
}
