#include <iostream>
#include <string>
#include <sstream>

using namespace std;

void printPascalTriangle(int n) { // Функция, печатающая треугольник Паскаля до степени n
    for (int line = 0; line <= n; line++) {
        int C = 1; // Первый кэффициент равен 1
        for (int i = 0; i <= line; i++) {
            cout << C << " ";
            C = C * (line - i) / (i + 1); // Формула для вычисления
        }
        cout << endl;
    }
}

int main(int argc, char* argv[]) { // Функция принимает аргументы командной строки
    int N = -1;
    bool flags[4] = { false }; // Массив для ключей b, c, d, e

    // Разбор аргументов
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];

        if (arg == "-a") {
            if (i + 1 < argc) { // Проверяем, есть ли следующий аргумент (число)
                string next_arg = argv[i + 1];
                stringstream ss(next_arg); // Строка - число
                if (!(ss >> N) || !ss.eof() || N < 0) {
                    cerr << "Error: N must be a non-negative integer" << endl;
                    return 1;
                }
                i++;
            }
            else {
                cerr << "Error: key -a requires argument N" << endl;
                return 1;
            }
        }
        else if (arg.size() == 2 && arg[0] == '-' && arg[1] >= 'b' && arg[1] <= 'e') {
            // Ключи -b, -c, -d, -e
            int idx = arg[1] - 'b'; // b->0, c->1, d->2, e->3
            flags[idx] = true;
        }
        else {
            cerr << "Unknown argument: " << arg << endl;
        }
    }
    //вывод информации об обработанных ключах
    const char* key_names = "bcde";
    for (int i = 0; i < 4; i++) {
        if (flags[i]) {
            cout << argv[0] << ": key -" << key_names[i] << " is set - processed" << endl;
        }
    }

    // построение треугольника
    if (N >= 0) {
        cout << "Pascal's triangle up to degree " << N << ":" << endl;
        printPascalTriangle(N);
    }
    else {
        cout << "Key -a is not set, triangle is not built." << endl;
    }

    return 0;
}