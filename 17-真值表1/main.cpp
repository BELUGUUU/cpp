// Problem H: 真值表(Ⅰ) 词法分析  OJ 2257
// 把逻辑表达式切分成词(token)，输出词序列、字母序变量表和变量取值组合
#include <iostream>
#include <string>
#include <vector>
using namespace std;

int main() {
    string line;
    while (getline(cin, line)) {
        while (!line.empty() && line[line.size() - 1] == '\r') line.erase(line.size() - 1);

        // ---- 词法分析：整体切词 ----
        vector<string> toks;
        int i = 0, n = (int)line.size();
        while (i < n) {
            char c = line[i];
            if (c == ' ') { i++; continue; }                        // 空格是分隔符
            if (c == '<' && i + 2 < n && line[i+1] == '-' && line[i+2] == '>') { toks.push_back("<->"); i += 3; }
            else if (c == '-' && i + 1 < n && line[i+1] == '>')    { toks.push_back("->");  i += 2; }
            else if (c == '|' && i + 1 < n && line[i+1] == '|')    { toks.push_back("||");  i += 2; }
            else                                                   { toks.push_back(string(1, c)); i++; }
        }

        // ---- 收集变量（去重后按字母序）----
        bool seen[26] = {false};
        for (size_t k = 0; k < toks.size(); k++)
            if (toks[k].size() == 1 && toks[k][0] >= 'a' && toks[k][0] <= 'z')
                seen[toks[k][0] - 'a'] = true;
        vector<int> vars;
        for (int k = 0; k < 26; k++) if (seen[k]) vars.push_back(k);

        // ---- 输出 ----
        for (size_t k = 0; k < toks.size(); k++) {
            if (k) cout << ' ';
            cout << toks[k];
        }
        cout << "\n";
        for (size_t k = 0; k < vars.size(); k++) {
            if (k) cout << ' ';
            cout << (char)('a' + vars[k]);
        }
        cout << "\n";
        int m = (int)vars.size();
        for (int mask = (1 << m) - 1; mask >= 0; mask--) {          // 从全 1 到全 0
            for (int j = 0; j < m; j++) {
                if (j) cout << ' ';
                cout << ((mask >> (m - 1 - j)) & 1);
            }
            cout << "\n";
        }
    }
    return 0;
}
