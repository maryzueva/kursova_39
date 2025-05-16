#include <iostream>
#include <vector>
#include <queue>
#include <functional>
#include <iomanip>
using namespace std;

// Основна функція розв’язання задачі на дошці без чисел
void solveFixedBoard(int mode) {
    int N;
    int field[10][10];

    if (mode == 1) {
        N = 5;
        int preset[5][5] = {
            {3, 0, 0, 0, 0},
            {0, 0, 0, 0, 3},
            {0, 0, 2, 0, 0},
            {0, 0, 0, 0, 0},
            {0, 2, 0, 0, 0}
        };
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                field[i][j] = preset[i][j];
    }
    else if (mode == 2) {
        cout << "Введіть розмір дошки N (1 ≤ N ≤ 5): ";
        if (!(cin >> N) || N <= 0 || N > 10) {
            cout << "Неправильне значення N.\n";
            return;
        }
        cout << "Введіть дошку (" << N << " x " << N << ", 0 для порожньо, ціле >0 — цифра):\n";
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j) {
                if (!(cin >> field[i][j]) || field[i][j] < 0) {
                    cout << "Неправильне значення. Завершення.\n";
                    return;
                }
            }
    } else {
        cout << "Невідомий режим.\n";
        return;
    }

    bool visited[10][10] = {};
    bool isBlack[10][10] = {};
    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};

    // Перевірка на вихід за межі
    auto inBounds = [&](int x, int y) {
        return x >= 0 && y >= 0 && x < N && y < N;
    };

    // Перевіряє, чи є чорний квадрат 2×2
    auto hasBlackSquare = [&]() {
        for (int i = 0; i < N - 1; ++i)
            for (int j = 0; j < N - 1; ++j)
                if (isBlack[i][j] && isBlack[i+1][j] && isBlack[i][j+1] && isBlack[i+1][j+1])
                    return true;
        return false;
    };

    // BFS-заповнення області. Перевіряє відповідність числу.
    function<int(int, int, vector<pair<int,int>>&)> floodFill = [&](int x, int y, vector<pair<int,int>>& area) {
        queue<pair<int,int>> q;
        q.push({x, y});
        visited[x][y] = true;
        int count = 0;
        int num = field[x][y];
        bool hasNum = num > 0;

        while (!q.empty()) {
            auto [cx, cy] = q.front(); q.pop();
            area.push_back({cx, cy});
            count++;

            for (int d = 0; d < 4; ++d) {
                int nx = cx + dx[d], ny = cy + dy[d];
                if (inBounds(nx, ny) && !visited[nx][ny] && !isBlack[nx][ny]) {
                    visited[nx][ny] = true;
                    if (field[nx][ny] > 0) {
                        if (hasNum) return -1; // більше ніж одне число в області
                        hasNum = true;
                        num = field[nx][ny];
                    }
                    q.push({nx, ny});
                }
            }
        }
        if (!hasNum) return -1;
        return count == num ? 1 : 0;
    };

    // Перевірка валідності розфарбування: усі області правильні, немає 2×2
    function<bool()> validate = [&]() {
        for (int i = 0; i < N; ++i)
            fill(visited[i], visited[i] + N, false);
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                if (!visited[i][j] && !isBlack[i][j]) {
                    vector<pair<int,int>> area;
                    int res = floodFill(i, j, area);
                    if (res != 1) return false;
                }
        return !hasBlackSquare();
    };

    // Рекурсивний перебір усіх можливих розфарбувань
    function<bool(int, int)> dfs = [&](int x, int y) -> bool {
        if (x == N) return validate();
        int nx = (y + 1 == N) ? x + 1 : x;
        int ny = (y + 1) % N;
        if (dfs(nx, ny)) return true;
        isBlack[x][y] = true;
        if (dfs(nx, ny)) return true;
        isBlack[x][y] = false;
        return false;
    };

    // Вивід дошки з кольоровим форматуванням
    auto print = [&]() {
        cout << "\n\033[1;37m    +" << string(N * 3, '-') << "+\n\033[0m";
        for (int i = 0; i < N; ++i) {
            cout << "\033[1;37m" << setw(2) << i << " |\033[0m";
            for (int j = 0; j < N; ++j) {
                if (isBlack[i][j]) cout << "\033[1;34m # \033[0m";
                else if (field[i][j] > 0) cout << "\033[1;32m" << setw(2) << field[i][j] << " \033[0m";
                else cout << " . ";
            }
            cout << "\033[1;37m|\033[0m\n";
        }
        cout << "\033[1;37m    +" << string(N * 3, '-') << "+\033[0m\n";
    };

    // Спроба знайти рішення
    if (dfs(0, 0)) {
        cout << "\033[1;32m\nРішення знайдено:\033[0m\n";
        print();
    } else {
        cout << "\033[1;31m\nРішення не існує.\033[0m\n";
    }
}