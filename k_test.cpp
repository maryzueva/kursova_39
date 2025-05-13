#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include <iomanip>
#include <numeric>
#include <thread>  // для std::this_thread::sleep_for
#include <chrono>  // для std::chrono::milliseconds

using namespace std;

#define RESET   "\033[0m"
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define BLUE    "\033[1;34m"

const int N = 12;

int grid[N][N] = {
    {5,3,3,3,3,6,6,4,4,4,4,2},
    {5,7,7,7,6,6,6,6,4,2,2,2},
    {5,5,7,7,4,4,4,6,4,2,5,5},
    {5,4,7,7,7,7,4,4,2,2,6,5},
    {5,4,4,5,5,5,5,7,7,2,6,5},
    {4,4,4,5,6,6,6,7,6,6,6,5},
    {4,6,4,5,5,6,7,7,7,6,6,5},
    {6,6,6,6,3,6,6,6,7,6,6,4},
    {6,6,6,6,3,3,6,6,7,7,6,4},
    {6,2,2,3,3,4,6,6,7,5,6,4},
    {6,2,2,4,4,4,7,7,7,5,4,4},
    {6,6,7,7,7,7,7,5,5,5,5,4}
};

enum CellState { UNKNOWN, WHITE, BLACK, ERROR };
CellState state[N][N];
bool visited[N][N];

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
    for (int j = 0; j < N; ++j)
        cout << setw(2) << j << " ";
    cout << "\n    +" << string(N * 3, '-') << "+\n";

    for (int i = 0; i < N; ++i) {
        cout << setw(2) << i << " |";
        for (int j = 0; j < N; ++j) {
            if (state[i][j] == WHITE)
                cout << GREEN << setw(2) << grid[i][j] << " " << RESET;
            else if (state[i][j] == BLACK)
                cout << BLUE << " # " << RESET;
            else if (state[i][j] == ERROR)
                cout << RED << " ! " << RESET;
            else
                cout << " . ";
        }
        cout << "|\n";
    }

    cout << "    +" << string(N * 3, '-') << "+\n";
    pause(80);
}

bool inBounds(int x, int y) {
    return x >= 0 && y >= 0 && x < N && y < N;
}

void clearVisited() {
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            visited[i][j] = false;
}

bool makes2x2(int x, int y) {
    if (!inBounds(x, y)) return false;
    state[x][y] = BLACK;
    bool result = false;

    for (int dx = -1; dx <= 0; ++dx) {
        for (int dy = -1; dy <= 0; ++dy) {
            int a = x + dx, b = y + dy;
            if (inBounds(a, b) && inBounds(a+1, b+1)) {
                if (state[a][b] == BLACK &&
                    state[a+1][b] == BLACK &&
                    state[a][b+1] == BLACK &&
                    state[a+1][b+1] == BLACK) {
                    result = true;
                }
            }
        }
    }

    state[x][y] = UNKNOWN;
    return result;
}

bool isConnected(const vector<pair<int,int>>& cells) {
    if (cells.empty()) return false;
    set<pair<int,int>> cellSet(cells.begin(), cells.end());
    queue<pair<int,int>> q;
    set<pair<int,int>> seen;

    q.push(cells[0]);
    seen.insert(cells[0]);

    while (!q.empty()) {
        auto [x, y] = q.front(); q.pop();
        for (int d = 0; d < 4; ++d) {
            int nx = x + dx[d], ny = y + dy[d];
            if (cellSet.count({nx, ny}) && !seen.count({nx, ny})) {
                seen.insert({nx, ny});
                q.push({nx, ny});
            }
        }
    }

    return seen.size() == cells.size();
}

bool chooseValidIsland(const vector<pair<int,int>>& group, int val, vector<pair<int,int>>& chosen) {
    vector<int> comb(group.size(), 0);
    fill(comb.end() - val, comb.end(), 1);

    do {
        vector<pair<int,int>> test;
        for (size_t i = 0; i < comb.size(); ++i)
            if (comb[i]) test.push_back(group[i]);

        if (isConnected(test)) {
            chosen = test;
            return true;
        }
    } while (next_permutation(comb.begin(), comb.end()));

    return false;
}

void dfsSameValue(int x, int y, int val, vector<pair<int,int>>& region) {
    if (!inBounds(x, y) || visited[x][y]) return;
    if (grid[x][y] != val || state[x][y] != UNKNOWN) return;

    visited[x][y] = true;
    region.push_back({x, y});

    for (int d = 0; d < 4; ++d)
        dfsSameValue(x + dx[d], y + dy[d], val, region);
}

void processWhiteRegions() {
    clearVisited();
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (!visited[i][j]) {
                vector<pair<int,int>> region;
                dfsSameValue(i, j, grid[i][j], region);
                int val = grid[i][j];

                if ((int)region.size() < val) {
                    for (auto [x, y] : region) {
                        state[x][y] = ERROR;
                        printFormattedGrid();
                    }
                } else {
                    vector<pair<int,int>> chosen;
                    if (chooseValidIsland(region, val, chosen)) {
                        for (auto [x, y] : region) {
                            if (find(chosen.begin(), chosen.end(), make_pair(x, y)) == chosen.end()) {
                                if (!makes2x2(x, y))
                                    state[x][y] = BLACK;
                                else
                                    state[x][y] = ERROR;
                                printFormattedGrid();
                            }
                        }
                        for (auto [x, y] : chosen) {
                            state[x][y] = WHITE;
                            printFormattedGrid();
                        }
                    } else {
                        for (auto [x, y] : region) {
                            state[x][y] = ERROR;
                            printFormattedGrid();
                        }
                    }
                }
            }
}

bool is2x2Black() {
    for (int i = 0; i < N - 1; ++i)
        for (int j = 0; j < N - 1; ++j)
            if (state[i][j] == BLACK && state[i+1][j] == BLACK &&
                state[i][j+1] == BLACK && state[i+1][j+1] == BLACK)
                return true;
    return false;
}

bool checkBlackConnected() {
    clearVisited();
    queue<pair<int,int>> q;
    bool found = false;
    int total = 0, seen = 0;

    for (int i = 0; i < N && !found; ++i)
        for (int j = 0; j < N; ++j)
            if (state[i][j] == BLACK) {
                q.push({i,j});
                visited[i][j] = true;
                found = true;
                break;
            }

    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (state[i][j] == BLACK)
                total++;

    while (!q.empty()) {
        auto [x, y] = q.front(); q.pop();
        seen++;
        for (int d = 0; d < 4; ++d) {
            int nx = x + dx[d], ny = y + dy[d];
            if (inBounds(nx, ny) && state[nx][ny] == BLACK && !visited[nx][ny]) {
                visited[nx][ny] = true;
                q.push({nx, ny});
            }
        }
    }

    return seen == total;
}

int main() {
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            state[i][j] = UNKNOWN;

    processWhiteRegions();

    bool squareError = is2x2Black();
    bool blackConnected = checkBlackConnected();

    printFormattedGrid();

    if (squareError) cout << RED << "⚠️  Помилка: є чорний блок 2×2\n" << RESET;
    if (!blackConnected) cout << RED << "⚠️  Помилка: чорна область не з’єднана\n" << RESET;
    if (!squareError && blackConnected) cout << GREEN << "✅ Успішне вирішення!\n" << RESET;

    return 0;
}
