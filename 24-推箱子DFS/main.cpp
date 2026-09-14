/*
 * 湖南科技大学 数据结构与算法课程设计   Problem X: 推箱子游戏-深度优先搜索版本
 * 题号：1982          语言：C++
 *
 * 思路：状态仍然是「玩家位置 + 箱子位置」。按题目要求用 DFS：
 *   每到一个状态，就按 上(U)、右(R)、下(D)、左(L) 的顺时针方向依次尝试，
 *   前一个方向失败才试下一个；一旦箱子被推到目的地就立即返回。
 *   走过的状态用四维数组标记，避免反复绕圈。
 *
 * 与 BFS 版的区别：本题不要求最少步数，只要求给出 DFS 实际走出的那一条路径，
 *   所以输出的是一个由 U/R/D/L 组成的移动序列；无解输出 -1。
 *
 * 输入：第一行 N M（0<N,M<=12），接下来 N 行 M 列，'.'空地 'X'玩家
 *   '*'箱子 '#'障碍 '@'目的地。
 *
 * 编译：g++ -O2 -std=c++11 -o main.exe main.cpp
 */
#include <cstdio>
#include <cstring>
#include <string>
using namespace std;

char mp[15][15];
int visited[13][13][13][13];
int N, M, gr, gc;
string answer;
bool found;

const int dr[4] = {-1, 0, 1, 0};
const int dc[4] = {0, 1, 0, -1};
const char dir[4] = {'U', 'R', 'D', 'L'};

void dfs(int pr, int pc, int br, int bc) {
    if (found) return;
    if (br == gr && bc == gc) { found = true; return; }

    for (int d = 0; d < 4 && !found; d++) {
        int nr = pr + dr[d], nc = pc + dc[d];
        if (nr < 0 || nr >= N || nc < 0 || nc >= M) continue;
        if (mp[nr][nc] == '#') continue;

        int nbr = br, nbc = bc;
        if (nr == br && nc == bc) {
            nbr = br + dr[d]; nbc = bc + dc[d];
            if (nbr < 0 || nbr >= N || nbc < 0 || nbc >= M) continue;
            if (mp[nbr][nbc] == '#') continue;
        }
        if (visited[nr][nc][nbr][nbc]) continue;

        visited[nr][nc][nbr][nbc] = 1;
        answer.push_back(dir[d]);
        dfs(nr, nc, nbr, nbc);
        if (found) return;          // 找到就立即返回，不再回溯
        answer.erase(answer.size() - 1);   // 该方向失败，撤销这一步
    }
}

int main() {
    while (scanf("%d %d", &N, &M) == 2) {
        int pr = 0, pc = 0, br = 0, bc = 0;
        for (int i = 0; i < N; i++) {
            scanf("%s", mp[i]);
            for (int j = 0; j < M; j++) {
                if (mp[i][j] == 'X') { pr = i; pc = j; }
                else if (mp[i][j] == '*') { br = i; bc = j; }
                else if (mp[i][j] == '@') { gr = i; gc = j; }
            }
        }

        memset(visited, 0, sizeof(visited));
        answer.clear();
        found = false;
        visited[pr][pc][br][bc] = 1;
        dfs(pr, pc, br, bc);

        if (found) printf("%s\n", answer.c_str());
        else printf("-1\n");
    }
    return 0;
}
