/*
 * 湖南科技大学 数据结构与算法课程设计   Problem S: 24点游戏(Ⅰ)
 * 题号：1829          语言：C++
 *
 * 题意：输入是一棵表达式树的先序遍历序列（'#' 表示空结点），
 *   判断这棵树的值能不能算出 24；能就输出全括号表达式并接上 "=24"，否则输出 NO。
 *
 * 思路：
 *   1. 先序序列递归建树：读一个记号，'#' 是空子树；是运算符就先递归左子树、
 *      再递归右子树；是数字就是叶子。
 *   2. 递归返回时顺便拼出全括号中缀式：out = "(" + 左式 + 运算符 + 右式 + ")"。
 *   3. 值用 double 计算（题目表达式来自真实 24 点场景，除法要按实数比较，
 *      用整数除法会误判），与 24 的误差小于 1e-9 认为相等。
 *
 * 编译：g++ -O2 -std=c++11 -o main.exe main.cpp
 */
#include <cstdio>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <string>
using namespace std;

char tokens[2005][16];
int cnt;

// 递归建树：返回该子树的值，out 为对应的全括号中缀表达式
double parse(int &idx, string &out) {
    const char *tk = tokens[idx++];
    if (tk[0] == '#') { out = ""; return 0; }

    if (strlen(tk) == 1 && (tk[0] == '+' || tk[0] == '-' ||
                            tk[0] == '*' || tk[0] == '/')) {
        string lo, ro;
        double a = parse(idx, lo);
        double b = parse(idx, ro);
        out = "(" + lo + string(tk) + ro + ")";
        if (tk[0] == '+') return a + b;
        if (tk[0] == '-') return a - b;
        if (tk[0] == '*') return a * b;
        return a / b;
    }
    out = tk;                     // 叶子是数字
    // 叶子结点在序列里还跟着两个 '#'（左右空孩子），要把它们读掉
    string skip1, skip2;
    parse(idx, skip1);
    parse(idx, skip2);
    return atof(tk);
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        cnt = 0;
        char *p = strtok(line, " \t\r\n");
        while (p != NULL && cnt < 2000) {
            strncpy(tokens[cnt], p, 15);
            tokens[cnt][15] = '\0';
            cnt++;
            p = strtok(NULL, " \t\r\n");
        }
        if (cnt == 0) continue;                 // 空行跳过

        int idx = 0;
        string expr;
        double val = parse(idx, expr);

        if (fabs(val - 24.0) < 1e-9) printf("%s=24\n", expr.c_str());
        else printf("NO\n");
    }
    return 0;
}
