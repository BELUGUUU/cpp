/*
 * 湖南科技大学 数据结构与算法课程设计   Problem W: 推箱子游戏-广度优先搜索版本
 * 题号：1981          语言：C++
 *
 * 思路：把「玩家位置 + 箱子位置」看成一个状态，玩家每走一步就是一次状态转移，
 *   用 BFS 逐层扩展，第一次让箱子落到目的地时走过的层数就是最少步数。
 *   状态总数 = 12*12*12*12 ≈ 2 万，BFS 完全够用。
 *
 * 状态转移（玩家移动方向 dr/dc）：
 *   1) 目标格越界或是墙 -> 非法；
 *   2) 目标格有箱子 -> 箱子要被推到再前面一格，那一格必须不越界、不是墙、
 *      且没有别的箱子；玩家和箱子一起移动；
 *   3) 目标格是空地/目的地 -> 只有玩家移动。
 *
 * 输入：第一行 N M（0<N,M<=12），接下来 N 行 M 列，'.'空地 'X'玩家
 *   '*'箱子 '#'障碍 '@'目的地。
 * 输出：最少步数；无法完成输出 -1。
 *
 * 编译：g++ -O2 -std=c++11 -o main.exe main.cpp
 */
#include <cstdio>
#include <cstring>
#include <queue>
using namespace std;

char mp[15][15];
int visited[13][13][13][13];   // [玩家行][玩家列][箱子行][箱子列]
int N, M;

struct State {
    int pr, pc, br, bc, step;
};

int main() {
    while (scanf("%d %d", &N, &M) == 2) {
        int pr = 0, pc = 0, br = 0, bc = 0, gr = 0, gc = 0;
        for (int i = 0; i < N; i++) {
            scanf("%s", mp[i]);
            for (int j = 0; j < M; j++) {
                if (mp[i][j] == 'X') { pr = i; pc = j; }
                else if (mp[i][j] == '*') { br = i; bc = j; }
                else if (mp[i][j] == '@') { gr = i; gc = j; }
            }
        }

        memset(visited, 0, sizeof(visited));
        const int dr[4] = {-1, 0, 1, 0};
        const int dc[4] = {0, 1, 0, -1};

        queue<State> q;
        State s0; s0.pr = pr; s0.pc = pc; s0.br = br; s0.bc = bc; s0.step = 0;
        q.push(s0);
        visited[pr][pc][br][bc] = 1;

        int answer = -1;
        while (!q.empty()) {
            State cur = q.front(); q.pop();

            if (cur.br == gr && cur.bc == gc) { answer = cur.step; break; }

            for (int d = 0; d < 4; d++) {
                int nr = cur.pr + dr[d], nc = cur.pc + dc[d];
                if (nr < 0 || nr >= N || nc < 0 || nc >= M) continue;   // 越界
                if (mp[nr][nc] == '#') continue;                        // 撞墙

                int nbr = cur.br, nbc = cur.bc;
                if (nr == cur.br && nc == cur.bc) {
                    // 要把箱子往同方向推一格
                    nbr = cur.br + dr[d]; nbc = cur.bc + dc[d];
                    if (nbr < 0 || nbr >= N || nbc < 0 || nbc >= M) continue;
                    if (mp[nbr][nbc] == '#') continue;
                }
                if (visited[nr][nc][nbr][nbc]) continue;
                visited[nr][nc][nbr][nbc] = 1;

                State ns;
                ns.pr = nr; ns.pc = nc; ns.br = nbr; ns.bc = nbc;
                ns.step = cur.step + 1;
                q.push(ns);
            }
        }
        printf("%d\n", answer);
    }
    return 0;
}
