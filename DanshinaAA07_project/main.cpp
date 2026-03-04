#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <windows.h>

using namespace std;

// ========== ЛОГГИРОВАНИЕ ==========
class Logger {
private:
    ofstream logFile;
    string getCurrentTime() {
        time_t now = time(nullptr);
        char buf[80];
        tm timeInfo;
        localtime_s(&timeInfo, &now);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeInfo);
        return string(buf);
    }

public:
    Logger(const string& filename = "log.txt") {
        logFile.open(filename, ios::app);
        if (logFile.is_open()) {
            logFile << "\n=== Программа запущена " << getCurrentTime() << " ===\n";
        }
    }

    ~Logger() {
        if (logFile.is_open()) {
            logFile << "=== Программа завершена " << getCurrentTime() << " ===\n\n";
            logFile.close();
        }
    }

    void log(const string& action, const string& details = "") {
        if (logFile.is_open()) {
            logFile << "[" << getCurrentTime() << "] " << action;
            if (!details.empty()) {
                logFile << " - " << details;
            }
            logFile << endl;
        }
    }
};

// ========== КЛАСС ДЛЯ ВВОДА ==========
class InputHandler {
private:
    static Logger* logger;

public:
    static void setLogger(Logger* log) {
        logger = log;
    }

    static void clearInputStream() {
        cin.clear();
        cin.ignore(10000, '\n');
    }

    static int getIntInput(const string& prompt, int minVal, int maxVal) {
        int value;
        while (true) {
            cout << prompt;
            if (cin >> value && value >= minVal && value <= maxVal) {
                clearInputStream();
                return value;
            }
            cout << "Ошибка! Введите число от " << minVal << " до " << maxVal << endl;
            clearInputStream();
        }
    }

    static double getDoubleInput(const string& prompt, double minVal) {
        double value;
        while (true) {
            cout << prompt;
            if (cin >> value && value >= minVal) {
                clearInputStream();
                return value;
            }
            cout << "Ошибка! Введите число >= " << minVal << endl;
            clearInputStream();
        }
    }

    static string getStringInput(const string& prompt) {
        cout << prompt;
        string value;

        // Очищаем буфер ТОЛЬКО если там есть что-то
        if (cin.peek() == '\n') {
            cin.ignore();
        }

        getline(cin, value);

        if (logger) logger->log("Введена строка", value);
        return value;
    }

    static bool getBoolInput(const string& prompt) {
        return getIntInput(prompt + " (1 - Да, 0 - Нет): ", 0, 1) == 1;
    }
};

Logger* InputHandler::logger = nullptr;

// ========== БАЗОВЫЙ КЛАСС ==========
class BaseObject {
protected:
    static int nextPipeId;
    static int nextStationId;
    int id;

public:
    BaseObject() : id(0) {}
    virtual ~BaseObject() {}
    int getId() const { return id; }
    virtual string getType() const = 0;
    virtual string getName() const = 0;
    virtual void display() const = 0;
    virtual string serialize() const = 0;
};

int BaseObject::nextPipeId = 1;
int BaseObject::nextStationId = 1;

// ========== КЛАСС ТРУБЫ ==========
class Pipe : public BaseObject {
private:
    string name;
    double length;
    int diameter;
    bool inRepair;

public:
    Pipe() : name(""), length(0), diameter(0), inRepair(false) {
        id = nextPipeId++;
    }

    void input() {
        name = InputHandler::getStringInput("Введите название трубы: ");
        length = InputHandler::getDoubleInput("Введите длину (км): ", 0);
        diameter = InputHandler::getIntInput("Введите диаметр (мм): ", 1, 5000);
        inRepair = InputHandler::getBoolInput("Труба в ремонте?");
    }

    void display() const override {
        cout << "\n--- ТРУБА ID: " << id << " ---\n";
        cout << "Название: " << name << endl;
        cout << "Длина: " << length << " км" << endl;
        cout << "Диаметр: " << diameter << " мм" << endl;
        cout << "В ремонте: " << (inRepair ? "Да" : "Нет") << endl;
        cout << "-------------------\n";
    }

    string getName() const override { return name; }
    string getType() const override { return "Pipe"; }
    bool isInRepair() const { return inRepair; }
    void toggleRepair() { inRepair = !inRepair; }

    string serialize() const override {
        return "PIPE|" + to_string(id) + "|" + name + "|" +
            to_string(length) + "|" + to_string(diameter) + "|" +
            (inRepair ? "1" : "0");
    }

    static Pipe deserialize(const string& data) {
        Pipe pipe;
        size_t pos = 0;
        string temp = data;

        pos = temp.find('|');
        temp = temp.substr(pos + 1);

        pos = temp.find('|');
        pipe.id = stoi(temp.substr(0, pos));
        temp = temp.substr(pos + 1);

        pos = temp.find('|');
        pipe.name = temp.substr(0, pos);
        temp = temp.substr(pos + 1);

        pos = temp.find('|');
        pipe.length = stod(temp.substr(0, pos));
        temp = temp.substr(pos + 1);

        pos = temp.find('|');
        pipe.diameter = stoi(temp.substr(0, pos));
        temp = temp.substr(pos + 1);

        pipe.inRepair = (temp == "1");

        return pipe;
    }

    static bool filterByName(const Pipe& pipe, const string& searchName) {
        return pipe.name.find(searchName) != string::npos;
    }

    static bool filterByRepair(const Pipe& pipe, bool repairStatus) {
        return pipe.inRepair == repairStatus;
    }
};

// ========== КЛАСС СТАНЦИИ ==========
class CompressorStation : public BaseObject {
private:
    string name;
    int totalWorkshops;
    int workingWorkshops;
    int stationClass;

public:
    CompressorStation() : name(""), totalWorkshops(0), workingWorkshops(0), stationClass(0) {
        id = nextStationId++;
    }

    void input() {
        name = InputHandler::getStringInput("Введите название станции: ");
        totalWorkshops = InputHandler::getIntInput("Введите общее количество цехов: ", 1, 1000);

        do {
            workingWorkshops = InputHandler::getIntInput("Введите количество работающих цехов: ", 0, totalWorkshops);
            if (workingWorkshops > totalWorkshops) {
                cout << "Ошибка! Работающих цехов не может быть больше общего количества.\n";
            }
        } while (workingWorkshops > totalWorkshops);

        stationClass = InputHandler::getIntInput("Введите класс станции: ", 1, 10);
    }

    void display() const override {
        double percentage = totalWorkshops > 0 ?
            (double)(totalWorkshops - workingWorkshops) / totalWorkshops * 100 : 0;

        cout << "\n--- КС ID: " << id << " ---\n";
        cout << "Название: " << name << endl;
        cout << "Всего цехов: " << totalWorkshops << endl;
        cout << "Работает: " << workingWorkshops << endl;
        cout << "Свободно: " << percentage << "%" << endl;
        cout << "Класс: " << stationClass << endl;
        cout << "-------------------\n";
    }

    string getName() const override { return name; }
    string getType() const override { return "CompressorStation"; }

    int getTotalWorkshops() const { return totalWorkshops; }
    int getWorkingWorkshops() const { return workingWorkshops; }

    double getFreeWorkshopsPercentage() const {
        if (totalWorkshops == 0) return 0;
        return (double)(totalWorkshops - workingWorkshops) / totalWorkshops * 100;
    }

    void startWorkshop() {
        if (workingWorkshops < totalWorkshops) {
            workingWorkshops++;
        }
    }

    void stopWorkshop() {
        if (workingWorkshops > 0) {
            workingWorkshops--;
        }
    }

    string serialize() const override {
        return "CS|" + to_string(id) + "|" + name + "|" +
            to_string(totalWorkshops) + "|" + to_string(workingWorkshops) + "|" +
            to_string(stationClass);
    }

    static CompressorStation deserialize(const string& data) {
        CompressorStation cs;
        size_t pos = 0;
        string temp = data;

        pos = temp.find('|');
        temp = temp.substr(pos + 1);

        pos = temp.find('|');
        cs.id = stoi(temp.substr(0, pos));
        temp = temp.substr(pos + 1);

        pos = temp.find('|');
        cs.name = temp.substr(0, pos);
        temp = temp.substr(pos + 1);

        pos = temp.find('|');
        cs.totalWorkshops = stoi(temp.substr(0, pos));
        temp = temp.substr(pos + 1);

        pos = temp.find('|');
        cs.workingWorkshops = stoi(temp.substr(0, pos));
        temp = temp.substr(pos + 1);

        cs.stationClass = stoi(temp);

        return cs;
    }

    static bool filterByName(const CompressorStation& cs, const string& searchName) {
        return cs.name.find(searchName) != string::npos;
    }

    static bool filterByFreePercentage(const CompressorStation& cs, double minPercentage) {
        return cs.getFreeWorkshopsPercentage() >= minPercentage;
    }
};

// ========== КЛАСС ДЛЯ УПРАВЛЕНИЯ КОЛЛЕКЦИЯМИ ==========
template<typename T>
class ObjectManager {
private:
    vector<T> objects;
    Logger* logger;

public:
    ObjectManager(Logger* log = nullptr) : logger(log) {}

    void add(const T& obj) {
        objects.push_back(obj);
        if (logger) logger->log("Добавлен объект", obj.getType() + " ID: " + to_string(obj.getId()));
    }

    bool removeById(int id) {
        for (auto it = objects.begin(); it != objects.end(); ++it) {
            if (it->getId() == id) {
                objects.erase(it);
                if (logger) logger->log("Удален объект", "ID: " + to_string(id));
                return true;
            }
        }
        return false;
    }

    T* findById(int id) {
        for (auto& obj : objects) {
            if (obj.getId() == id) {
                return &obj;
            }
        }
        return nullptr;
    }

    vector<T*> findByFilter(bool (*filter)(const T&, const string&), const string& param) {
        vector<T*> result;
        for (auto& obj : objects) {
            if (filter(obj, param)) {
                result.push_back(&obj);
            }
        }
        return result;
    }

    vector<T*> findByFilter(bool (*filter)(const T&, bool), bool param) {
        vector<T*> result;
        for (auto& obj : objects) {
            if (filter(obj, param)) {
                result.push_back(&obj);
            }
        }
        return result;
    }

    vector<T*> findByFilter(bool (*filter)(const T&, double), double param) {
        vector<T*> result;
        for (auto& obj : objects) {
            if (filter(obj, param)) {
                result.push_back(&obj);
            }
        }
        return result;
    }

    void displayAll() const {
        if (objects.empty()) {
            cout << "Нет объектов для отображения.\n";
            return;
        }
        for (const auto& obj : objects) {
            obj.display();
        }
    }

    size_t size() const { return objects.size(); }

    void saveToFile(const string& filename) const {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "Ошибка открытия файла для записи!\n";
            return;
        }

        for (const auto& obj : objects) {
            file << obj.serialize() << endl;
        }

        file.close();
        if (logger) logger->log("Сохранение в файл", filename);
    }

    void loadFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Ошибка открытия файла для чтения!\n";
            return;
        }

        objects.clear();
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;

            if (line.length() >= 4 && line.substr(0, 4) == "PIPE") {
                objects.push_back(T::deserialize(line));
            }
        }

        file.close();
        if (logger) logger->log("Загрузка из файла", filename + " (загружено " + to_string(objects.size()) + " объектов)");
    }

    vector<int> getAllIds() const {
        vector<int> ids;
        for (const auto& obj : objects) {
            ids.push_back(obj.getId());
        }
        return ids;
    }
};

// Специализация для CompressorStation
template<>
void ObjectManager<CompressorStation>::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Ошибка открытия файла для чтения!\n";
        return;
    }

    objects.clear();
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        if (line.length() >= 2 && line.substr(0, 2) == "CS") {
            objects.push_back(CompressorStation::deserialize(line));
        }
    }

    file.close();
    if (logger) logger->log("Загрузка из файла", filename + " (загружено " + to_string(objects.size()) + " объектов)");
}

// ========== ГЛАВНЫЙ КЛАСС ПРИЛОЖЕНИЯ ==========
class PipelineApp {
private:
    ObjectManager<Pipe> pipeManager;
    ObjectManager<CompressorStation> csManager;
    Logger logger;
    bool running;

    void setRussian() {
        SetConsoleCP(1251);
        SetConsoleOutputCP(1251);
    }

    void displayMainMenu() {
        cout << "\n========== МЕНЮ ==========\n";
        cout << "1. Управление трубами\n";
        cout << "2. Управление КС\n";
        cout << "3. Показать все объекты\n";
        cout << "4. Сохранить в файл\n";
        cout << "5. Загрузить из файла\n";
        cout << "0. Выход\n";
        cout << "==========================\n";
        cout << "Выберите действие: ";
    }

    void pipeMenu() {
        while (true) {
            cout << "\n--- УПРАВЛЕНИЕ ТРУБАМИ ---\n";
            cout << "1. Добавить трубу\n";
            cout << "2. Просмотреть все трубы\n";
            cout << "3. Редактировать трубу\n";
            cout << "4. Удалить трубу\n";
            cout << "5. Поиск труб\n";
            cout << "6. Пакетное редактирование\n";
            cout << "0. Назад\n";
            cout << "--------------------------\n";

            int choice = InputHandler::getIntInput("Выберите действие: ", 0, 6);

            switch (choice) {
            case 1: {
                Pipe pipe;
                pipe.input();
                pipeManager.add(pipe);
                break;
            }
            case 2:
                pipeManager.displayAll();
                break;
            case 3: {
                int id = InputHandler::getIntInput("Введите ID трубы: ", 1, 9999);
                Pipe* pipe = pipeManager.findById(id);
                if (pipe) {
                    pipe->toggleRepair();
                    logger.log("Изменен статус ремонта трубы", "ID: " + to_string(id));
                    cout << "Статус ремонта изменен!\n";
                }
                else {
                    cout << "Труба не найдена!\n";
                }
                break;
            }
            case 4: {
                int id = InputHandler::getIntInput("Введите ID трубы для удаления: ", 1, 9999);
                if (pipeManager.removeById(id)) {
                    cout << "Труба удалена!\n";
                }
                else {
                    cout << "Труба не найдена!\n";
                }
                break;
            }
            case 5:
                searchPipes();
                break;
            case 6:
                batchEditPipes();
                break;
            case 0:
                return;
            }
        }
    }

    void csMenu() {
        while (true) {
            cout << "\n--- УПРАВЛЕНИЕ КС ---\n";
            cout << "1. Добавить КС\n";
            cout << "2. Просмотреть все КС\n";
            cout << "3. Редактировать КС\n";
            cout << "4. Удалить КС\n";
            cout << "5. Поиск КС\n";
            cout << "0. Назад\n";
            cout << "---------------------\n";

            int choice = InputHandler::getIntInput("Выберите действие: ", 0, 5);

            switch (choice) {
            case 1: {
                CompressorStation cs;
                cs.input();
                csManager.add(cs);
                break;
            }
            case 2:
                csManager.displayAll();
                break;
            case 3: {
                int id = InputHandler::getIntInput("Введите ID КС: ", 1, 9999);
                CompressorStation* cs = csManager.findById(id);
                if (cs) {
                    cout << "1. Запустить цех\n2. Остановить цех\n";
                    int action = InputHandler::getIntInput("Выберите действие: ", 1, 2);
                    if (action == 1) {
                        cs->startWorkshop();
                        logger.log("Запущен цех на КС", "ID: " + to_string(id));
                    }
                    else {
                        cs->stopWorkshop();
                        logger.log("Остановлен цех на КС", "ID: " + to_string(id));
                    }
                    cout << "КС обновлена!\n";
                }
                else {
                    cout << "КС не найдена!\n";
                }
                break;
            }
            case 4: {
                int id = InputHandler::getIntInput("Введите ID КС для удаления: ", 1, 9999);
                if (csManager.removeById(id)) {
                    cout << "КС удалена!\n";
                }
                else {
                    cout << "КС не найдена!\n";
                }
                break;
            }
            case 5:
                searchCS();
                break;
            case 0:
                return;
            }
        }
    }

    void searchPipes() {
        cout << "\n--- ПОИСК ТРУБ ---\n";
        cout << "1. По названию\n";
        cout << "2. По статусу ремонта\n";

        int choice = InputHandler::getIntInput("Выберите тип поиска: ", 1, 2);

        if (choice == 1) {
            string name = InputHandler::getStringInput("Введите название для поиска: ");
            auto results = pipeManager.findByFilter(Pipe::filterByName, name);
            cout << "Найдено труб: " << results.size() << "\n";
            for (auto pipe : results) {
                pipe->display();
            }
            logger.log("Поиск труб по названию", name);
        }
        else {
            bool repairStatus = InputHandler::getBoolInput("Искать трубы в ремонте?");
            auto results = pipeManager.findByFilter(Pipe::filterByRepair, repairStatus);
            cout << "Найдено труб: " << results.size() << "\n";
            for (auto pipe : results) {
                pipe->display();
            }
            logger.log("Поиск труб по статусу ремонта", repairStatus ? "в ремонте" : "не в ремонте");
        }
    }

    void searchCS() {
        cout << "\n--- ПОИСК КС ---\n";
        cout << "1. По названию\n";
        cout << "2. По проценту свободных цехов\n";

        int choice = InputHandler::getIntInput("Выберите тип поиска: ", 1, 2);

        if (choice == 1) {
            string name = InputHandler::getStringInput("Введите название для поиска: ");
            auto results = csManager.findByFilter(CompressorStation::filterByName, name);
            cout << "Найдено КС: " << results.size() << "\n";
            for (auto cs : results) {
                cs->display();
            }
            logger.log("Поиск КС по названию", name);
        }
        else {
            double percentage = InputHandler::getDoubleInput("Введите минимальный процент свободных цехов: ", 0);
            auto results = csManager.findByFilter(CompressorStation::filterByFreePercentage, percentage);
            cout << "Найдено КС: " << results.size() << "\n";
            for (auto cs : results) {
                cs->display();
            }
            logger.log("Поиск КС по проценту свободных цехов", to_string(percentage));
        }
    }

    void batchEditPipes() {
        if (pipeManager.size() == 0) {
            cout << "Нет труб для редактирования!\n";
            return;
        }

        cout << "\n--- ПАКЕТНОЕ РЕДАКТИРОВАНИЕ ТРУБ ---\n";
        cout << "1. Редактировать все трубы\n";
        cout << "2. Редактировать по списку ID\n";

        int choice = InputHandler::getIntInput("Выберите режим: ", 1, 2);
        vector<int> ids;

        if (choice == 1) {
            ids = pipeManager.getAllIds();
        }
        else {
            cout << "Введите ID труб (0 - закончить):\n";
            while (true) {
                int id = InputHandler::getIntInput("ID: ", 0, 9999);
                if (id == 0) break;
                ids.push_back(id);
            }
        }

        if (ids.empty()) {
            cout << "Не выбрано ни одной трубы!\n";
            return;
        }

        cout << "Что сделать с выбранными трубами?\n";
        cout << "1. Переключить статус ремонта\n";
        cout << "2. Удалить\n";

        int action = InputHandler::getIntInput("Выберите действие: ", 1, 2);

        if (action == 1) {
            for (int id : ids) {
                Pipe* pipe = pipeManager.findById(id);
                if (pipe) {
                    pipe->toggleRepair();
                }
            }
            cout << "Статус ремонта изменен для " << ids.size() << " труб\n";
            logger.log("Пакетное изменение статуса ремонта",
                "Изменено труб: " + to_string(ids.size()));
        }
        else {
            for (int id : ids) {
                pipeManager.removeById(id);
            }
            cout << "Удалено труб: " << ids.size() << "\n";
            logger.log("Пакетное удаление труб",
                "Удалено труб: " + to_string(ids.size()));
        }
    }

    void saveToFile() {
        string filename = InputHandler::getStringInput("Введите имя файла для сохранения: ");

        pipeManager.saveToFile(filename + "_pipes.txt");
        csManager.saveToFile(filename + "_cs.txt");

        cout << "Данные сохранены в файлы: " << filename << "_pipes.txt и " << filename << "_cs.txt\n";
        logger.log("Сохранение данных", filename);
    }

    void loadFromFile() {
        string filename = InputHandler::getStringInput("Введите имя файла для загрузки: ");

        pipeManager.loadFromFile(filename + "_pipes.txt");
        csManager.loadFromFile(filename + "_cs.txt");

        cout << "Данные загружены!\n";
        cout << "Загружено труб: " << pipeManager.size() << "\n";
        cout << "Загружено КС: " << csManager.size() << "\n";
    }

    void displayAll() {
        cout << "\n=== ВСЕ ОБЪЕКТЫ ===\n";
        cout << "\n--- ТРУБЫ ---\n";
        pipeManager.displayAll();
        cout << "\n--- КОМПРЕССОРНЫЕ СТАНЦИИ ---\n";
        csManager.displayAll();
    }

public:
    PipelineApp() : logger("log.txt"), running(true) {
        setRussian();
        InputHandler::setLogger(&logger);
        logger.log("Приложение запущено");
    }

    ~PipelineApp() {
        logger.log("Приложение завершено");
    }

    void run() {
        while (running) {
            displayMainMenu();
            int choice = InputHandler::getIntInput("", 0, 5);

            switch (choice) {
            case 1:
                pipeMenu();
                break;
            case 2:
                csMenu();
                break;
            case 3:
                displayAll();
                break;
            case 4:
                saveToFile();
                break;
            case 5:
                loadFromFile();
                break;
            case 0:
                running = false;
                cout << "Программа завершена.\n";
                break;
            }
        }
    }
};

// ========== ТОЧКА ВХОДА ==========
int main() {
    PipelineApp app;
    app.run();
    return 0;
}