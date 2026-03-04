#include <iostream> //  библиотека для ввода-вывода
#include <string> // библиотка для работы со строками
using namespace std;

struct Pipe { // (структура, чтобы объединить все х-ки трубы в одном типе данных)
    string name; // название трубы
    float length; //длина 
    int diameter; // диаметр 
    bool repair; // признак "в ремонте"
};

struct CompressorStation { //так же, для компрессорной станция
    string name; // название станции
    int totalWorkshops; // общее количество цехов
    int workingWorkshops; // количество работающих цехов, не больше общего кол-ва!!
    int stationClass; // класс станции
};

void displayPipe(const Pipe& pipe) { //вывод информации о трубе, конст ссылка - не изменим данные
    cout << "Pipe: " << pipe.name << endl;
    cout << "Length: " << pipe.length << " km" << endl;
    cout << "Diameter: " << pipe.diameter << " mm" << endl;
    cout << "In repair: " << (pipe.repair ? "Yes" : "No") << endl;
    cout << "-------------------" << endl;
}

void displayCS(const CompressorStation& cs) { //вывод информации о станции
    cout << "Compressor Station: " << cs.name << endl;
    cout << "Total workshops: " << cs.totalWorkshops << endl;
    cout << "Working workshops: " << cs.workingWorkshops << endl;
    cout << "Station class: " << cs.stationClass << endl;
    cout << "-------------------" << endl;
}

void inputPipe(Pipe& pipe) { //ввод данных о трубе
    cout << "Enter pipe name: ";
    cin.ignore(); //очистка ввода (чтобы убрать \n)
    getline(cin, pipe.name);//чтение строки с пробелами

    cout << "Enter length (km): "; // защита от некорректного ввода
    while (!(cin >> pipe.length) || pipe.length <= 0) {
        cout << "Invalid input. Enter positive number: ";
        cin.clear(); //сброс
        cin.ignore(10000, '\n'); // очистка ощибочного ввода 
    }

    cout << "Enter diameter (mm): ";
    while (!(cin >> pipe.diameter) || pipe.diameter <= 0) {
        cout << "Invalid input. Enter positive number: ";
        cin.clear();
        cin.ignore(10000, '\n');
    }

    cout << "Is pipe in repair? (1 - Yes, 0 - No): ";
    while (!(cin >> pipe.repair) || (pipe.repair != 0 && pipe.repair != 1)) {
        cout << "Invalid input. Enter 1 or 0: ";
        cin.clear();
        cin.ignore(10000, '\n');
    }
}

void inputCS(CompressorStation& cs) { // ввода данных о станции
    cout << "Enter station name: ";
    cin.ignore();
    getline(cin, cs.name);
    // проверка,что работающих цехов не больше общего количества
    cout << "Enter total number of workshops: ";
    while (!(cin >> cs.totalWorkshops) || cs.totalWorkshops <= 0) {
        cout << "Invalid input. Enter positive number: ";
        cin.clear();
        cin.ignore(10000, '\n');
    }

    cout << "Enter number of working workshops: ";
    while (!(cin >> cs.workingWorkshops) || cs.workingWorkshops < 0 || cs.workingWorkshops > cs.totalWorkshops) { // + проверка, меньше кол-ва цехов
        cout << "Invalid input. Enter number between 0 and " << cs.totalWorkshops << ": ";
        cin.clear();
        cin.ignore(10000, '\n');
    }

    cout << "Enter station class: ";
    while (!(cin >> cs.stationClass) || cs.stationClass <= 0) {
        cout << "Invalid input. Enter positive number: ";
        cin.clear();
        cin.ignore(10000, '\n');
    }
}

void editPipeRepair(Pipe& pipe) { // ремонт
    cout << "Current repair status: " << (pipe.repair ? "In repair" : "Not in repair") << endl;
    cout << "Change repair status? (1 - Yes, 0 - No): ";
    int choice;
    while (!(cin >> choice) || (choice != 0 && choice != 1)) {
        cout << "Invalid input. Enter 1 or 0: ";
        cin.clear();
        cin.ignore(10000, '\n');
    }

    if (choice == 1) {
        pipe.repair = !pipe.repair;// инверт. значения
        cout << "Repair status changed to: " << (pipe.repair ? "In repair" : "Not in repair") << endl;
    }
}

void editCSWorkshops(CompressorStation& cs) { //упарвление цехами станции
    cout << "Current working workshops: " << cs.workingWorkshops << " out of " << cs.totalWorkshops << endl;
    cout << "1. Start workshop\n2. Stop workshop\n3. Cancel\nChoose option: ";

    int choice;
    while (!(cin >> choice) || choice < 1 || choice > 3) {
        cout << "Invalid input. Enter 1, 2 or 3: ";
        cin.clear();
        cin.ignore(10000, '\n');
    }

    if (choice == 1) { //нельзя запустить, если все уже в работе
        if (cs.workingWorkshops < cs.totalWorkshops) {
            cs.workingWorkshops++; // запуск цеха
            cout << "Workshop started. Now " << cs.workingWorkshops << " workshops working." << endl;
        }
        else {
            cout << "All workshops are already working!" << endl;
        }
    }
    else if (choice == 2) { // то же самое с остановкой
        if (cs.workingWorkshops > 0) {
            cs.workingWorkshops--; // остановка цеха
            cout << "Workshop stopped. Now " << cs.workingWorkshops << " workshops working." << endl;
        }
        else {
            cout << "No workshops are working!" << endl;
        }
    }
}

void Menu() {
    Pipe pipe;
    CompressorStation cs;
    bool pipeExists = false; // флаг существования трубы 
    bool csExists = false; // флаг существования станции

    while (true) {  // беконечный цикл меню
        cout << "\nMENU " << endl;
        cout << "1. Add pipe" << endl;
        cout << "2. Add compressor station" << endl;
        cout << "3. View all objects" << endl;
        cout << "4. Edit pipe" << endl;
        cout << "5. Edit compressor station" << endl;
        cout << "0. Exit" << endl;
        cout << "Choose option: ";

        int option;
        cin >> option;

        switch (option) {
        case 1:
            inputPipe(pipe);
            pipeExists = true; //установка флага после добавления
            break;

        case 2:
            inputCS(cs);
            csExists = true;
            break;

        case 3:
            if (pipeExists) {
                displayPipe(pipe);
            }
            else {
                cout << "No pipe data available." << endl;
            }

            if (csExists) {
                displayCS(cs);
            }
            else {
                cout << "No compressor station data available." << endl;
            }
            break;

        case 4:
            if (pipeExists) { // проверка перед выводом
                editPipeRepair(pipe);
            }
            else {
                cout << "No pipe data available. Please add pipe first." << endl;
            }
            break;

        case 5:
            if (csExists) {
                editCSWorkshops(cs);
            }
            else {
                cout << "No compressor station data available. Please add CS first." << endl;
            }
            break;

        case 0:
            cout << "Exiting program." << endl;
            return;

        default:
            cout << "Invalid option. Please try again." << endl;
            cin.clear();
            cin.ignore(10000, '\n');
            break;
        }
    }
}

int main() {
    Menu(); // запуск главного меню
    return 0; // завершение программы
}