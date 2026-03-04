#include <iostream>
#include <fstream>
#include <vector>
#include <thread> //для многопоточности
#include <mutex> //синхронизация потоков
#include <chrono> //измерение времени

std::mutex cout_mutex;

// поразрядная сортировка 
void radixSort(std::vector<int>& arr) {
    // 3 разряда
    for (int exp = 1; exp <= 100; exp *= 10) {
        std::vector<int> output(arr.size());
        int count[10] = { 0 };

        // подсчет цифр (извлекаем и идем дальше)
        for (int num : arr) {
            count[(num / exp) % 10]++;
        }

        // преобразование счетчиков в позиции
        for (int i = 1; i < 10; i++) {
            count[i] += count[i - 1];
        }

        //отсортированный массив
        for (int i = arr.size() - 1; i >= 0; i--) {
            int digit = (arr[i] / exp) % 10;
            output[count[digit] - 1] = arr[i]; //ставим на правильную позицию 
            count[digit]--;
        }

        arr = output;
    }
}

// вывод массива в формате [a, b, c, ... , x, y, z] для визуализации
void printArrayFormatted(const std::vector<int>& arr) {
    std::cout << "["
        << arr[0] << ", " << arr[1] << ", " << arr[2] << ", " << arr[3] << ", " << arr[4]
        << ", ..., "
        << arr[arr.size() - 5] << ", " << arr[arr.size() - 4] << ", "
        << arr[arr.size() - 3] << ", " << arr[arr.size() - 2] << ", " << arr[arr.size() - 1]
        << "]";
}

// обработка файла
void processFile(const std::string& filename) {
    // чтение файла + засекаем время
    auto start_read = std::chrono::high_resolution_clock::now();

    std::ifstream file(filename);
    if (!file) {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cerr << "Error opening: " << filename << std::endl;
        return;
    }

    std::vector<int> originalData;
    int num;
    while (file >> num) {
        originalData.push_back(num);
    }
    file.close();

    auto end_read = std::chrono::high_resolution_clock::now();
    auto read_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_read - start_read); //разница во времени
    // Искусственная задержка для анализа в VMMap
    std::this_thread::sleep_for(std::chrono::seconds(10));
    // копия для сортировки
    std::vector<int> sortedData = originalData;

    // сортировка
    auto start_sort = std::chrono::high_resolution_clock::now();
    radixSort(sortedData);
    auto end_sort = std::chrono::high_resolution_clock::now();
    auto sort_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_sort - start_sort);

    // вывод
    std::lock_guard<std::mutex> lock(cout_mutex); //блокируем вывод

    std::cout << "Original data from " << filename << ":" << std::endl;
    std::cout << " ";
    printArrayFormatted(originalData);
    std::cout << std::endl;

    std::cout << "Sorted data from " << filename << ":" << std::endl;
    std::cout << " ";
    printArrayFormatted(sortedData);
    std::cout << std::endl;

    std::cout << "File: " << filename
        << " - Size: " << originalData.size() << " elements" << std::endl;
    std::cout << "Read time: " << read_duration.count() << " microseconds" << std::endl;
    std::cout << "Sort time: " << sort_duration.count() << " microseconds" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "Radix Sort" << std::endl;

    //  ДО создания потоков 
    // задержка для анализа: программа запущена, но потоки еще не созданы
    std::cout << "\n=== POINT 1: Program started ===" << std::endl;
    std::cout << "Threads are NOT created yet" << std::endl;
    std::cout << "Switch to VMMap and analyze the process" << std::endl;
    std::cout << "Press Enter to continue..." << std::endl;
    std::cin.get();  // ПАУЗА 1 - анализируем в VMMap

    //проверка ввода
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " file1.txt file2.txt file3.txt" << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return 1;
    }

    auto program_start = std::chrono::high_resolution_clock::now(); //начало общего отсчета

    // запуск потоков
    std::vector<std::thread> threads;
    for (int i = 1; i < argc; i++) {
        threads.emplace_back(processFile, argv[i]);
    }

    // ВО ВРЕМЯ работы потоков 
    // задержка для анализа: потоки созданы и работают в фоновом режиме
    std::cout << "\n=== POINT 2: Threads created and running ===" << std::endl;
    std::cout << "Created " << (argc - 1) << " threads" << std::endl;
    std::cout << "Switch to VMMap and analyze the process (press F5 to refresh)" << std::endl;
    std::cout << "Press Enter after analysis..." << std::endl;
    std::cin.get();  // ПАУЗА 2 - анализируем в VMMap

    // ждем пока каждый поток закончит работу
    for (auto& t : threads) {
        t.join();
    }

    auto program_end = std::chrono::high_resolution_clock::now();
    auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(program_end - program_start);

    std::cout << "Total execution time: " << total_duration.count() << " milliseconds" << std::endl;

    // ПОСЛЕ завершения потоков 
    // задержка для анализа в VMMap: все потоки завершены, программа готовится к выходу
    std::cout << "\n=== POINT 3: All threads completed ===" << std::endl;
    std::cout << "Program finished." << std::endl;
    std::cout << "Switch to VMMap for final memory analysis" << std::endl;
    std::cout << "Press Enter to exit program..." << std::endl;
    std::cin.get();  // ПАУЗА 3 - финальный анализ в VMMap

    return 0;
}