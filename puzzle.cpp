#include "puzzle.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include <thread>
#include <chrono>
using namespace std;

const int Puzzle::DX[DIRS] = {1, -1, 0, 0};
const int Puzzle::DY[DIRS] = {0, 0, 1, -1};

// Призупиняє виконання програми на певний час (для візуального ефекту)
void Puzzle::pause(int ms) const {
    this_thread::sleep_for(chrono::milliseconds(ms));
}

// Перевіряє, чи координати в межах сітки
bool Puzzle::inBounds(int x, int y) const {
    return x >= 0 && y >= 0 && x < n && y < m;
}

// Скидає візитні мітки перед новим обходом
void Puzzle::clearVis() {
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j)
            vis[i][j] = false;
}

// Виводить поточний стан сітки з кольоровим оформленням
void Puzzle::print() const {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    cout << "\n     ";
    for (int j = 0; j < m; ++j)
        cout << setw(2) << j << " ";
    cout << "\n    +" << string(m * 3, '-') << "+\n";
    for (int i = 0; i < n; ++i) {
        cout << setw(2) << i << " |";
        for (int j = 0; j < m; ++j) {
            if (col[i][j] == WHITE)
                cout << "\033[1;32m" << setw(2) << mat[i][j] << " \033[0m";
            else if (col[i][j] == BLACK)
                cout << "\033[1;34m # \033[0m";
            else if (col[i][j] == ERROR_STATE)
                cout << "\033[1;31m ! \033[0m";
            else
                cout << " . ";
        }
        cout << "|\n";
    }
    cout << "    +" << string(m * 3, '-') << "+\n";
    pause(PAUSE_MS);
}

// Тимчасово фарбує клітинку чорним і перевіряє, чи утворюється 2×2 чорний квадрат
bool Puzzle::forms2x2(int x, int y) {
    col[x][y] = BLACK;
    bool found = false;
    for (int dx0 = -1; dx0 <= 0; ++dx0)
        for (int dy0 = -1; dy0 <= 0; ++dy0) {
            int a = x + dx0, b = y + dy0;
            if (inBounds(a,b) && inBounds(a+1,b+1)) {
                if (col[a][b] == BLACK && col[a+1][b] == BLACK &&
                    col[a][b+1] == BLACK && col[a+1][b+1] == BLACK) {
                    found = true;
                }
            }
        }
    col[x][y] = UNKNOWN;
    return found;
}

// Перевіряє наявність чорного блоку 2x2
bool Puzzle::has2x2() const {
    for (int i = 0; i < n - 1; ++i)
        for (int j = 0; j < m - 1; ++j)
            if (col[i][j] == BLACK && col[i+1][j] == BLACK &&
                col[i][j+1] == BLACK && col[i+1][j+1] == BLACK)
                return true;
    return false;
}

// Перевіряє, чи всі чорні клітинки утворюють зв'язну область
bool Puzzle::blackConnected() {
    clearVis();
    queue<pair<int,int>> q;
    bool started = false;
    int total = 0, seen = 0;

    for (int i = 0; i < n && !started; ++i)
        for (int j = 0; j < m; ++j)
            if (col[i][j] == BLACK) {
                q.push({i,j});
                vis[i][j] = true;
                started = true;
                break;
            }

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j)
            if (col[i][j] == BLACK) total++;

    while (!q.empty()) {
        auto [x,y] = q.front(); q.pop();
        seen++;
        for (int d = 0; d < DIRS; ++d) {
            int nx = x + DX[d], ny = y + DY[d];
            if (inBounds(nx,ny) && col[nx][ny] == BLACK && !vis[nx][ny]) {
                vis[nx][ny] = true;
                q.push({nx,ny});
            }
        }
    }

    return seen == total;
}

// Знаходить область однакових значень за допомогою DFS
void Puzzle::dfs(int x, int y, int val, vector<pair<int,int>>& reg) {
    if (!inBounds(x,y) || vis[x][y] || mat[x][y] != val || col[x][y] != UNKNOWN)
        return;
    vis[x][y] = true;
    reg.emplace_back(x,y);
    for (int d = 0; d < DIRS; ++d)
        dfs(x + DX[d], y + DY[d], val, reg);
}

// Перевіряє, чи всі клітинки зі списку утворюють одну зв'язну область
bool Puzzle::isGroupConn(const vector<pair<int,int>>& cells) const {
    if (cells.empty()) return false;
    set<pair<int,int>> all(cells.begin(), cells.end());
    queue<pair<int,int>> q;
    set<pair<int,int>> seen;
    q.push(cells[0]); seen.insert(cells[0]);

    while (!q.empty()) {
        auto [x,y] = q.front(); q.pop();
        for (int d = 0; d < DIRS; ++d) {
            int nx = x + DX[d], ny = y + DY[d];
            if (all.count({nx,ny}) && !seen.count({nx,ny})) {
                seen.insert({nx,ny}); q.push({nx,ny});
            }
        }
    }

    return seen.size() == cells.size();
}

// Вибирає зв'язну підмножину розміру val із заданої групи
bool Puzzle::chooseIsland(const vector<pair<int,int>>& group, int val, vector<pair<int,int>>& pick) const {
    vector<int> sel(group.size(), 0);
    fill(sel.end() - val, sel.end(), 1);
    do {
        vector<pair<int,int>> test;
        for (size_t i = 0; i < sel.size(); ++i)
            if (sel[i]) test.push_back(group[i]);
        if (isGroupConn(test)) {
            pick = test;
            return true;
        }
    } while (next_permutation(sel.begin(), sel.end()));
    return false;
}

// Обробляє всі білі області, розфарбовуючи їх згідно правил
void Puzzle::processWhite() {
    clearVis();
    for (int i = 0; i < n; ++i) for (int j = 0; j < m; ++j) {
        if (!vis[i][j] && col[i][j] == UNKNOWN) {
            vector<pair<int,int>> reg;
            dfs(i, j, mat[i][j], reg);
            int val = mat[i][j];
            if ((int)reg.size() < val) {
                for (auto &p: reg) { col[p.first][p.second] = ERROR_STATE; print(); }
            } else {
                vector<pair<int,int>> pick;
                if (chooseIsland(reg, val, pick)) {
                    for (auto &p: reg) {
                        if (find(pick.begin(), pick.end(), p) == pick.end()) {
                            if (!forms2x2(p.first,p.second)) col[p.first][p.second] = BLACK;
                            else col[p.first][p.second] = ERROR_STATE;
                            print();
                        }
                    }
                    for (auto &p: pick) {
                        col[p.first][p.second] = WHITE;
                        print();
                    }
                } else {
                    for (auto &p: reg) { col[p.first][p.second] = ERROR_STATE; print(); }
                }
            }
        }
    }
}

// Зчитує сітку з консольного вводу
bool Puzzle::input() {
    return input(cin);
}

// Зчитує сітку з переданого потоку вводу
bool Puzzle::input(istream& in) {
    if (!(in >> n >> m) || n <= 0 || m <= 0 || n > MAXN || m > MAXN) {
        cerr << "Невірні розміри сітки.\n";
        return false;
    }

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j)
            if (!(in >> mat[i][j]) || mat[i][j] <= 0 || mat[i][j] > n * m) {
                cerr << "Невірне значення у клітинці [" << i << "," << j << "].\n";
                return false;
            }

    for (int i = 0; i < MAXN; ++i)
        for (int j = 0; j < MAXN; ++j)
            col[i][j] = UNKNOWN;

    return true;
}

// Запускає розв'язання задачі та виводить результат
void Puzzle::solve(int z) {
    cout << "Розв'язання задачі #" << z << ":\n";
    processWhite();
    bool err2x2 = has2x2();
    bool blkConn = blackConnected();
    print();
    if (err2x2) cout << "\033[1;31mПомилка: чорний блок 2×2\033[0m\n";
    if (!blkConn) cout << "\033[1;31mПомилка: чорна область не є зв'язаною\033[0m\n";
    if (!err2x2 && blkConn) cout << "\033[1;32mРозв'язання успішне!\033[0m\n";
}