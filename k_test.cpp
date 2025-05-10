#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

const int N = 5;
int grid[N][N];
bool white[N][N]; // true - біла клітина
bool painted[N][N]; // true - чорна клітина

int dx[] = {-1, 1, 0, 0};
int dy[] = {0, 0, -1, 1};

// Перевірка валідності заливки
bool isValid(int x, int y) {
    return x >= 0 && x < N && y >= 0 && y < N && !white[x][y] && grid[x][y] == 0;
}

// Перевірка квадрата 2x2 серед чорних клітин
bool check2x2() {
    for (int i = 0; i < N-1; i++) {
        for (int j = 0; j < N-1; j++) {
            if (painted[i][j] && painted[i+1][j] && painted[i][j+1] && painted[i+1][j+1])
                return false;
        }
    }
    return true;
}

// Перевірка зв'язності чорних клітин
bool checkConnected() {
    bool visited[N][N] = {{false}};
    queue<pair<int,int>> q;
    bool found = false;

    for (int i = 0; i < N && !found; i++)
        for (int j = 0; j < N && !found; j++)
            if (painted[i][j]) {
                q.push({i, j});
                visited[i][j] = true;
                found = true;
            }

    if (!found) return true;

    while (!q.empty()) {
        auto [x, y] = q.front();
        q.pop();

        for (int d = 0; d < 4; d++) {
            int nx = x + dx[d], ny = y + dy[d];
            if (nx >= 0 && nx < N && ny >= 0 && ny < N && painted[nx][ny] && !visited[nx][ny]) {
                visited[nx][ny] = true;
                q.push({nx, ny});
            }
        }
    }

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (painted[i][j] && !visited[i][j])
                return false;

    return true;
}

// Друк поля: числа і чорні клітини
void printGrid() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (white[i][j])
                cout << grid[i][j];
            else
                cout << '#';
        }
        cout << endl;
    }
    cout << endl;
}

// Спроба залити область навколо (x,y) на value клітин
bool tryFillArea(int x, int y, int value, vector<pair<int,int>>& filled) {
    queue<pair<int,int>> q;
    q.push({x, y});
    filled.push_back({x, y});
    int cnt = 1; // рахунок білих клітин

    while (!q.empty() && cnt < value) {
        auto [cx, cy] = q.front();
        q.pop();

        vector<pair<int,int>> neighbors;
        for (int d = 0; d < 4; d++) {
            int nx = cx + dx[d];
            int ny = cy + dy[d];
            if (isValid(nx, ny)) {
                neighbors.push_back({nx, ny});
            }
        }

        sort(neighbors.begin(), neighbors.end(), [&](pair<int,int> a, pair<int,int> b) {
            int afree = 0, bfree = 0;
            for (int d = 0; d < 4; d++) {
                int ax = a.first + dx[d];
                int ay = a.second + dy[d];
                int bx = b.first + dx[d];
                int by = b.second + dy[d];
                if (isValid(ax, ay)) afree++;
                if (isValid(bx, by)) bfree++;
            }
            return afree < bfree;
        });

        for (auto& p : neighbors) {
            if (cnt == value) break;
            white[p.first][p.second] = true;
            cnt++;
            filled.push_back(p);
            q.push(p);
        }
    }

    return cnt == value;
}

// Відкат області
void undoFillArea(vector<pair<int,int>>& filled) {
    for (auto& p : filled) {
        if (grid[p.first][p.second] == 0)
            white[p.first][p.second] = false;
    }
}

// Основна функція з покроковим виводом
bool solvePuzzle() {
    bool allWhite = true;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (grid[i][j] > 0 && !white[i][j])
                allWhite = false;

    if (allWhite) {
        // Фарбування залишку чорним
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                if (!white[i][j])
                    painted[i][j] = true;

        return check2x2() && checkConnected();
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (grid[i][j] > 0 && !white[i][j]) {
                vector<pair<int,int>> filled;
                white[i][j] = true;
                filled.push_back({i,j});
                if (tryFillArea(i, j, grid[i][j], filled)) {
                    cout << "Заповнили область для числа " << grid[i][j] << " з точки (" << i << ", " << j << "):" << endl;
                    printGrid();
                    if (solvePuzzle())
                        return true;
                }
                undoFillArea(filled);
                white[i][j] = false;
            }
        }
    }

    return false;
}

int main() {
    int inputGrid[5][5] = {
        {5,5,5,5,5},
        {3,3,5,1,5},
        {5,5,5,5,5},
        {5,1,1,3,3},
        {5,5,2,2,3}
    };

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            if (inputGrid[i][j] > 0)
                grid[i][j] = inputGrid[i][j];
            else
                grid[i][j] = 0;
            white[i][j] = false;
            painted[i][j] = false;
        }

    if (solvePuzzle()) {
        cout << "Рішення знайдено:" << endl;
        printGrid();
    } else {
        cout << "Рішення не знайдено!" << endl;
    }

    return 0;
}