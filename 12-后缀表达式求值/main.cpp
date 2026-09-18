// Problem N: 后缀表达式求值  OJ 1824
// 栈始终保存尚未参与运算的数字；运算符取栈顶两个数并把结果压回。
// 逐字符扫描：数字进栈；遇运算符弹两个数计算。"*8" 这类粘连写法可自然分开
#include <iostream>
#include <stack>
#include <cctype>
#include <string>
using namespace std;

int main() {
    string line;
    while (getline(cin, line)) {
        stack<long long> st;
        size_t i = 0;
        while (i < line.size()) {
            char c = line[i];
            if (isspace((unsigned char)c)) { i++; continue; }
            if (c == '@') break;                    // 输入结束标志
            if (isdigit((unsigned char)c)) {
                long long v = 0;
                while (i < line.size() && isdigit((unsigned char)line[i])) {
                    v = v * 10 + line[i] - '0';
                    i++;
                }
                st.push(v);
            } else {                             // c 是一个二元运算符
                long long b = st.top(); st.pop();
                long long a = st.top(); st.pop();
                long long r = 0;
                if (c == '+') r = a + b;
                else if (c == '-') r = a - b;
                else if (c == '*') r = a * b;
                else r = a / b;
                st.push(r);
                i++;
            }
        }
        if (!st.empty()) cout << st.top() << "\n";
    }
    return 0;
}
