#include "puzzle.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

void solveFixedBoard(int mode);

int main() {
    while (true) {
        cout << "\n\033[1;36m====== ГОЛОВНЕ МЕНЮ ======\033[0m\n";
        cout << "1. Вибір типу задачі\n";
        cout << "2. Вийти\n";
        cout << "Ваш вибір: ";

        int mainChoice;
        if (!(cin >> mainChoice)) {
            cout << "Некоректний ввід. Спробуйте ще раз.\n";
            cin.clear(); cin.ignore(1000, '\n');
            continue;
        }

        if (mainChoice == 2) {
            cout << "Завершення програми.\n";
            break;
        } else if (mainChoice == 1) {
            cout << "\n\033[1;35m--- Виберіть тип задачі ---\033[0m\n";
            cout << "1. Дошка з числами\n";
            cout << "2. Дошка без чисел\n";
            cout << "Ваш вибір: ";

            int typeChoice;
            if (!(cin >> typeChoice)) {
                cout << "Некоректний ввід. Спробуйте ще раз.\n";
                cin.clear(); cin.ignore(1000, '\n');
                continue;
            }

            /* --------------------------------------------------
                    1) ДОШКА З ЧИСЛАМИ
               --------------------------------------------------*/
            if (typeChoice == 1) {
                cout << "\n\033[1;33m--- Дошка з числами ---\033[0m\n";
                cout << "1.Ввід вручну\n";
                cout << "2. Використати вбудовану задачу\n";                // ★ NEW
                cout << "Ваш вибір: ";

                int mode;
                if (!(cin >> mode)) {
                    cout << "Некоректний ввід. Спробуйте ще раз.\n";
                    cin.clear(); cin.ignore(1000, '\n');
                    continue;
                }
                /* ---------- 2) Ввід вручну ---------- */
                else if (mode == 1) {
                    int t;
                    cout << "Введіть кількість задач (T): ";
                    if (!(cin >> t) || t <= 0 || t > 100) {
                        cout << "Невірний ввід кількості задач. Спробуйте знову.\n";
                        continue;
                    }
                    for (int z = 1; z <= t; ++z) {
                        cout << "\nЗадача #" << z << ":\n";
                        cout << "Введіть розміри сітки (n m), потім саму сітку n×m:\n";
                        Puzzle p;
                        if (!p.input()) {
                            cout << "\033[1;31mПомилка вводу. Пропускаємо задачу.\033[0m\n";
                            continue;
                        }
                        p.solve(z);
                    }
                }
                /* ---------- 3) Вбудована задача ---------- */   // ★ NEW
                else if (mode == 2) {
                    const char *raw =
                        "12 12\n"
                        "5 3 3 3 3 6 6 4 4 4 4 2\n"
                        "5 7 7 7 6 6 6 6 4 2 2 2\n"
                        "5 5 7 7 4 4 4 6 4 2 5 5\n"
                        "5 4 7 7 7 7 4 4 2 2 6 5\n"
                        "5 4 4 5 5 5 5 7 7 2 6 5\n"
                        "4 4 4 5 6 6 6 7 6 6 6 5\n"
                        "4 6 4 5 5 6 7 7 7 6 6 5\n"
                        "6 6 6 6 3 6 6 6 7 6 6 4\n"
                        "6 6 6 6 3 3 6 6 7 7 6 4\n"
                        "6 2 2 3 3 4 6 6 7 5 6 4\n"
                        "6 2 2 4 4 4 7 7 7 5 4 4\n"
                        "6 6 7 7 7 7 7 5 5 5 5 4\n";
                    istringstream ss(raw);
                    Puzzle p;
                    if (!p.input(ss)) {
                        cout << "\033[1;31mПомилка у вбудованій задачі.\033[0m\n";
                    } else {
                        p.solve(1);
                    }
                }
                else {
                    cout << "Невідомий режим.\n";
                }
            }

            /* --------------------------------------------------
                    2) ДОШКА БЕЗ ЧИСЕЛ
               --------------------------------------------------*/
            else if (typeChoice == 2) {
                cout << "\n\033[1;33m--- Дошка без чисел ---\033[0m\n";
                cout << "1. Використати вбудовану дошку\n";
                cout << "2. Ввести дошку вручну\n";
                cout << "Ваш вибір: ";

                int fixedMode;
                if (!(cin >> fixedMode) || (fixedMode != 1 && fixedMode != 2)) {
                    cout << "Некоректний вибір режиму.\n";
                    continue;
                }
                solveFixedBoard(fixedMode);
            }
            else {
                cout << "Невідомий тип задачі.\n";
            }
        } 
        else {
            cout << "Невідомий вибір. Спробуйте знову.\n";
        }
    }
    return 0;
}