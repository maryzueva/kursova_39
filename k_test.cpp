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
int N, M;
int grid[MAXN][MAXN];
CellState state[MAXN][MAXN];
bool visited[MAXN][MAXN];

const int dx[4] = {1, -1, 0, 0};
const int dy[4] = {0, 0, 1, -1};

void pause(int ms) {
    this_thread::sleep_for(chrono::milliseconds(ms));
}

void printFormattedGrid() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    cout << "\n     ";
    for (int j = 0; j < M; ++j)
        cout << setw(2) << j << " ";
    cout << "\n    +" << string(M * 3, '-') << "+\n";
    for (int i = 0; i < N; ++i) {
        cout << setw(2) << i << " |";
        for (int j = 0; j < M; ++j) {
            if (state[i][j] == WHITE)
                cout << GREEN << setw(2) << grid[i][j] << " " << RESET;
            else if (state[i][j] == BLACK)
                cout << BLUE << " # " << RESET;
            else if (state[i][j] == ERROR_STATE)
                cout << RED << " ! " << RESET;
            else
                cout << " . ";
        }
        cout << "|\n";
    }
    cout << "    +" << string(M * 3, '-') << "+\n";
    pause(80);
}

bool inBounds(int x, int y) {
    return x >= 0 && y >= 0 && x < N && y < M;
}

void clearVisited() {
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < M; ++j)
            visited[i][j] = false;
}

bool makes2x2(int x, int y) {
    state[x][y] = BLACK;
    bool found = false;
    for (int dx0 = -1; dx0 <= 0; ++dx0)
        for (int dy0 = -1; dy0 <= 0; ++dy0) {
            int a = x + dx0, b = y + dy0;
            if (inBounds(a,b) && inBounds(a+1,b+1)) {
                if (state[a][b] == BLACK && state[a+1][b] == BLACK &&
                    state[a][b+1] == BLACK && state[a+1][b+1] == BLACK) {
                    found = true;
                }
            }
        }
    state[x][y] = UNKNOWN;
    return found;
}

bool is2x2Black() {
    for (int i = 0; i < N - 1; ++i)
        for (int j = 0; j < M - 1; ++j)
            if (state[i][j] == BLACK && state[i+1][j] == BLACK &&
                state[i][j+1] == BLACK && state[i+1][j+1] == BLACK)
                return true;
    return false;
}

bool checkBlackConnected() {
    clearVisited();
    queue<pair<int,int>> q;
    bool started = false;
    int total = 0, seen = 0;
    for (int i = 0; i < N && !started; ++i)
        for (int j = 0; j < M; ++j)
            if (state[i][j] == BLACK) {
                q.push({i,j});
                visited[i][j] = true;
                started = true;
                break;
            }
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < M; ++j)
            if (state[i][j] == BLACK) total++;
    while (!q.empty()) {
        auto [x,y] = q.front(); q.pop();
        seen++;
        for (int d = 0; d < 4; ++d) {
            int nx = x + dx[d], ny = y + dy[d];
            if (inBounds(nx,ny) && state[nx][ny] == BLACK && !visited[nx][ny]) {
                visited[nx][ny] = true;
                q.push({nx,ny});
            }
        }
    }
    return seen == total;
}

void dfsSameValue(int x, int y, int val, vector<pair<int,int>>& region) {
    if (!inBounds(x,y) || visited[x][y] || grid[x][y] != val || state[x][y] != UNKNOWN)
        return;
    visited[x][y] = true;
    region.emplace_back(x,y);
    for (int d = 0; d < 4; ++d)
        dfsSameValue(x + dx[d], y + dy[d], val, region);
}

bool isConnected(const vector<pair<int,int>>& cells) {
    if (cells.empty()) return false;
    set<pair<int,int>> cellSet(cells.begin(), cells.end());
    queue<pair<int,int>> q;
    set<pair<int,int>> seen;
    q.push(cells[0]); seen.insert(cells[0]);
    while (!q.empty()) {
        auto [x,y] = q.front(); q.pop();
        for (int d = 0; d < 4; ++d) {
            int nx = x + dx[d], ny = y + dy[d];
            if (cellSet.count({nx,ny}) && !seen.count({nx,ny})) {
                seen.insert({nx,ny}); q.push({nx,ny});
            }
        }
    }
    return seen.size() == cells.size();
}

bool chooseValidIsland(const vector<pair<int,int>>& group, int val, vector<pair<int,int>>& chosen) {
    vector<int> sel(group.size(), 0);
    fill(sel.end() - val, sel.end(), 1);
    do {
        vector<pair<int,int>> test;
        for (size_t i = 0; i < sel.size(); ++i)
            if (sel[i]) test.push_back(group[i]);
        if (isConnected(test)) {
            chosen = test;
            return true;
        }
    } while (next_permutation(sel.begin(), sel.end()));
    return false;
}

void processWhiteRegions() {
    clearVisited();
    for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) visited[i][j] = false;
    for (int i = 0; i < N; ++i) for (int j = 0; j < M; ++j) {
        if (!visited[i][j] && state[i][j] == UNKNOWN) {
            vector<pair<int,int>> region;
            dfsSameValue(i, j, grid[i][j], region);
            int val = grid[i][j];
            if ((int)region.size() < val) {
                for (auto &p: region) { state[p.first][p.second] = ERROR_STATE; printFormattedGrid(); }
            } else {
                vector<pair<int,int>> chosen;
                if (chooseValidIsland(region, val, chosen)) {
                    for (auto &p: region) {
                        if (find(chosen.begin(), chosen.end(), p) == chosen.end()) {
                            if (!makes2x2(p.first,p.second)) state[p.first][p.second] = BLACK;
                            else state[p.first][p.second] = ERROR_STATE;
                            printFormattedGrid();
                        }
                    }
                    for (auto &p: chosen) {
                        state[p.first][p.second] = WHITE;
                        printFormattedGrid();
                    }
                } else {
                    for (auto &p: region) { state[p.first][p.second] = ERROR_STATE; printFormattedGrid(); }
                }
            }
        }
    }
}

int main() {
    int T;
    cout << "Введіть кількість задач (T): ";
    if (!(cin >> T)) {
        cout << "Невірний ввід. Завершення програми." << endl;
        return 0;
    }
    
    for (int puzzle = 1; puzzle <= T; ++puzzle) {
        cout << "Задача #" << puzzle << ":\n";
        cout << "Введіть розміри сітки (N M): ";
        cin >> N >> M;
        
        cout << "Введіть елементи сітки (кожен рядок через пробіл):\n";
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < M; ++j) {
                cin >> grid[i][j];
            }
        }
        for (int i = 0; i < MAXN; ++i) {
            for (int j = 0; j < MAXN; ++j) {
                state[i][j] = UNKNOWN;
            }
        }

        cout << "Розв'язання задачі #" << puzzle << ":\n";
        processWhiteRegions();
        bool squareErr = is2x2Black();
        bool blackConn = checkBlackConnected();
        printFormattedGrid();
        
        if (squareErr) cout << RED << "Помилка: існує чорний блок 2×2" << RESET << "\n";
        if (!blackConn) cout << RED << "Помилка: чорна область не є зв'язаною" << RESET << "\n";
        if (!squareErr && blackConn) cout << GREEN << "Розв'язання успішне!" << RESET << "\n";
        cout << "\n";
    }

    return 0;
}
