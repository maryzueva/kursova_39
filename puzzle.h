#ifndef PUZZLE_H
#define PUZZLE_H

#include <vector>
#include <utility>
#include <iostream>

enum CellState { UNKNOWN, WHITE, BLACK, ERROR_STATE };

class Puzzle {
private:
    static const int MAXN = 12;
    static const int DIRS = 4;
    static const int DX[DIRS], DY[DIRS];
    static const int PAUSE_MS = 80;

    int n, m;
    int mat[MAXN][MAXN];
    CellState col[MAXN][MAXN];
    bool vis[MAXN][MAXN];

    void pause(int ms) const;
    void print() const;
    bool inBounds(int x, int y) const;
    void clearVis();
    bool forms2x2(int x, int y);
    bool has2x2() const;
    bool blackConnected();
    void dfs(int x, int y, int val, std::vector<std::pair<int,int>>& reg);
    bool isGroupConn(const std::vector<std::pair<int,int>>& cells) const;
    bool chooseIsland(const std::vector<std::pair<int,int>>& group, int val, std::vector<std::pair<int,int>>& pick) const;
    void processWhite();

public:
    bool input();
    bool input(std::istream& in);
    void solve(int z);
};

#endif