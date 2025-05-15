#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include <iomanip>
#include <thread>
#include <chrono>
#include <string>
#include <cstdlib>
using namespace std;

#define RESET   "\033[0m"
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define BLUE    "\033[1;34m"

enum CellState { UNKNOWN, WHITE, BLACK, ERROR_STATE };

const int MAXN = 12;
int g_n, g_m;
int g_mat[MAXN][MAXN];
CellState g_col[MAXN][MAXN];
bool g_vis[MAXN][MAXN];

const int DX[4] = {1, -1, 0, 0};
const int DY[4] = {0, 0, 1, -1};

void pause(int ms) {
    this_thread::sleep_for(chrono::milliseconds(ms));
}

void printGrid() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    cout << "\n     ";
    for (int j = 0; j < g_m; ++j)
        cout << setw(2) << j << " ";
    cout << "\n    +" << string(g_m * 3, '-') << "+\n";
    for (int i = 0; i < g_n; ++i) {
        cout << setw(2) << i << " |";
        for (int j = 0; j < g_m; ++j) {
            if (g_col[i][j] == WHITE)
                cout << GREEN << setw(2) << g_mat[i][j] << " " << RESET;
            else if (g_col[i][j] == BLACK)
                cout << BLUE << " # " << RESET;
            else if (g_col[i][j] == ERROR_STATE)
                cout << RED << " ! " << RESET;
            else
                cout << " . ";
        }
        cout << "|\n";
    }
    cout << "    +" << string(g_m * 3, '-') << "+\n";
    pause(80);
}

bool inBounds(int x, int y) {
    return x >= 0 && y >= 0 && x < g_n && y < g_m;
}

void clearVis() {
    for (int i = 0; i < g_n; ++i)
        for (int j = 0; j < g_m; ++j)
            g_vis[i][j] = false;
}

bool forms2x2(int x, int y) {
    g_col[x][y] = BLACK;
    bool found = false;
    for (int dx0 = -1; dx0 <= 0; ++dx0)
        for (int dy0 = -1; dy0 <= 0; ++dy0) {
            int a = x + dx0, b = y + dy0;
            if (inBounds(a,b) && inBounds(a+1,b+1)) {
                if (g_col[a][b] == BLACK && g_col[a+1][b] == BLACK &&
                    g_col[a][b+1] == BLACK && g_col[a+1][b+1] == BLACK) {
                    found = true;
                }
            }
        }
    g_col[x][y] = UNKNOWN;
    return found;
}

bool has2x2() {
    for (int i = 0; i < g_n - 1; ++i)
        for (int j = 0; j < g_m - 1; ++j)
            if (g_col[i][j] == BLACK && g_col[i+1][j] == BLACK &&
                g_col[i][j+1] == BLACK && g_col[i+1][j+1] == BLACK)
                return true;
    return false;
}

bool blackConnected() {
    clearVis();
    queue<pair<int,int>> q;
    bool started = false;
    int total = 0, seen = 0;
    for (int i = 0; i < g_n && !started; ++i)
        for (int j = 0; j < g_m; ++j)
            if (g_col[i][j] == BLACK) {
                q.push({i,j});
                g_vis[i][j] = true;
                started = true;
                break;
            }
    for (int i = 0; i < g_n; ++i)
        for (int j = 0; j < g_m; ++j)
            if (g_col[i][j] == BLACK) total++;
    while (!q.empty()) {
        auto [x,y] = q.front(); q.pop();
        seen++;
        for (int d = 0; d < 4; ++d) {
            int nx = x + DX[d], ny = y + DY[d];
            if (inBounds(nx,ny) && g_col[nx][ny] == BLACK && !g_vis[nx][ny]) {
                g_vis[nx][ny] = true;
                q.push({nx,ny});
            }
        }
    }
    return seen == total;
}

/*  ---------------------------------------------------------------------[<]-
    Функція: dfs
    Призначення: Пошук клітинок із тим самим значенням.
 ---------------------------------------------------------------------[>]-*/
void dfs(int x, int y, int val, vector<pair<int,int>>& reg) {
    if (!inBounds(x,y) || g_vis[x][y] || g_mat[x][y] != val || g_col[x][y] != UNKNOWN)
        return;
    g_vis[x][y] = true;
    reg.emplace_back(x,y);
    for (int d = 0; d < 4; ++d)
        dfs(x + DX[d], y + DY[d], val, reg);
}

/*  ---------------------------------------------------------------------[<]-
    Функція: isGroupConn
    Призначення: Перевіряє, чи клітинки утворюють зв'язну групу.
 ---------------------------------------------------------------------[>]-*/
bool isGroupConn(const vector<pair<int,int>>& cells) {
    if (cells.empty()) return false;
    set<pair<int,int>> all(cells.begin(), cells.end());
    queue<pair<int,int>> q;
    set<pair<int,int>> seen;
    q.push(cells[0]); seen.insert(cells[0]);
    while (!q.empty()) {
        auto [x,y] = q.front(); q.pop();
        for (int d = 0; d < 4; ++d) {
            int nx = x + DX[d], ny = y + DY[d];
            if (all.count({nx,ny}) && !seen.count({nx,ny})) {
                seen.insert({nx,ny}); q.push({nx,ny});
            }
        }
    }
    return seen.size() == cells.size();
}

/*  ---------------------------------------------------------------------[<]-
    Функція: chooseIsland
    Призначення: Вибирає зв'язну підгрупу клітинок розміром val.
 ---------------------------------------------------------------------[>]-*/
bool chooseIsland(const vector<pair<int,int>>& group, int val, vector<pair<int,int>>& pick) {
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

/*  ---------------------------------------------------------------------[<]-
    Функція: processWhite
    Призначення: Обробляє всі білі зони згідно правил.
 ---------------------------------------------------------------------[>]-*/
void processWhite() {
    clearVis();
    for (int i = 0; i < g_n; ++i) for (int j = 0; j < g_m; ++j) {
        if (!g_vis[i][j] && g_col[i][j] == UNKNOWN) {
            vector<pair<int,int>> reg;
            dfs(i, j, g_mat[i][j], reg);
            int val = g_mat[i][j];
            if ((int)reg.size() < val) {
                for (auto &p: reg) { g_col[p.first][p.second] = ERROR_STATE; printGrid(); }
            } else {
                vector<pair<int,int>> pick;
                if (chooseIsland(reg, val, pick)) {
                    for (auto &p: reg) {
                        if (find(pick.begin(), pick.end(), p) == pick.end()) {
                            if (!forms2x2(p.first,p.second)) g_col[p.first][p.second] = BLACK;
                            else g_col[p.first][p.second] = ERROR_STATE;
                            printGrid();
                        }
                    }
                    for (auto &p: pick) {
                        g_col[p.first][p.second] = WHITE;
                        printGrid();
                    }
                } else {
                    for (auto &p: reg) { g_col[p.first][p.second] = ERROR_STATE; printGrid(); }
                }
            }
        }
    }
}

int main() {
    int t;
    cout << "Введіть кількість задач (T): ";
    if (!(cin >> t)) {
        cout << "Невірний ввід. Завершення програми." << endl;
        return 0;
    }
    for (int z = 1; z <= t; ++z) {
        cout << "Задача #" << z << ":\n";
        cout << "Введіть розміри сітки (N M): ";
        cin >> g_n >> g_m;
        cout << "Введіть елементи сітки (рядки через пробіл):\n";
        for (int i = 0; i < g_n; ++i)
            for (int j = 0; j < g_m; ++j)
                cin >> g_mat[i][j];
        for (int i = 0; i < MAXN; ++i)
            for (int j = 0; j < MAXN; ++j)
                g_col[i][j] = UNKNOWN;

        cout << "Розв'язання задачі #" << z << ":\n";
        processWhite();
        bool err2x2 = has2x2();
        bool blkConn = blackConnected();
        printGrid();

        if (err2x2) cout << RED << "Помилка: чорний блок 2×2" << RESET << "\n";
        if (!blkConn) cout << RED << "Помилка: чорна область не є зв'язаною" << RESET << "\n";
        if (!err2x2 && blkConn) cout << GREEN << "Розв'язання успішне!" << RESET << "\n";
        cout << "\n";
    }
    return 0;
}
