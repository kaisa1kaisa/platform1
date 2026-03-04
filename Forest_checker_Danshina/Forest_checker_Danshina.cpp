#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <locale>

using namespace std;

class Edge {
public:
    int node1;
    int node2;
    Edge(int n1, int n2) : node1(n1), node2(n2) {}
};

class Node {
public:
    int id;
    vector<Edge*> edges;
    Node(int node_id) : id(node_id) {}
};

class ForestChecker {
private:
    unordered_map<int, Node*>& graph;
    bool hasCycleFlag;

public:
    ForestChecker(unordered_map<int, Node*>& g) : graph(g), hasCycleFlag(false) {}

    // Поиск циклов с помощью DFS
    void hasCycleDFS(int node, int parent, unordered_set<int>& visited, vector<int>& cyclePath) {
        visited.insert(node);
        cyclePath.push_back(node);

        for (Edge* edge : graph[node]->edges) {
            int neighbor = (edge->node1 == node) ? edge->node2 : edge->node1;

            if (!visited.count(neighbor)) {
                hasCycleDFS(neighbor, node, visited, cyclePath);
                if (hasCycleFlag) return;
            }
            else if (neighbor != parent) {
                cyclePath.push_back(neighbor);
                hasCycleFlag = true;
                return;
            }
        }

        if (!hasCycleFlag) {
            cyclePath.pop_back();
        }
    }

    // Поиск ВСЕХ циклов в графе
    vector<vector<int>> findAllCycles() {
        vector<vector<int>> allCycles;
        unordered_set<int> visited;

        for (auto& pair : graph) {
            int nodeId = pair.first;
            if (!visited.count(nodeId)) {
                vector<int> cyclePath;
                unordered_set<int> localVisited;
                hasCycleFlag = false;

                hasCycleDFS(nodeId, -1, visited, cyclePath);
                if (hasCycleFlag && cyclePath.size() >= 3) {
                    allCycles.push_back(cyclePath);
                }
            }
        }

        return allCycles;
    }

    // Проверка, является ли граф лесом
    void checkIsForest() {
        vector<vector<int>> cycles = findAllCycles();
        int hasNoCycles = cycles.empty();

        int n = graph.size();
        int m = countEdges();
        int k = countConnectedComponents();

        int satisfiesCondition = (m == n - k);

        cout << "Проверка условия леса: m = n - k" << endl;
        cout << "  Ребер (m): " << m << ", Вершин (n): " << n << ", Компонент (k): " << k << endl;
        cout << "  " << m << " = " << n << " - " << k << " = " << (n - k) << " : "
            << (satisfiesCondition ? "ДА" : "НЕТ") << endl;
        cout << "  Циклов найдено: " << cycles.size() << " : "
            << (hasNoCycles ? "НЕТ" : "ДА") << endl;

        cout << "  Результат: граф " << (hasNoCycles && satisfiesCondition ? "ЯВЛЯЕТСЯ" : "НЕ ЯВЛЯЕТСЯ") << " лесом" << endl;
    }

    // Подсчет общего количества ребер
    int countEdges() {
        int totalEdges = 0;
        for (auto& pair : graph) {
            totalEdges += pair.second->edges.size();
        }
        return totalEdges / 2;
    }

    // Подсчет компонент связности с помощью DFS
    int countConnectedComponents() {
        unordered_set<int> visited;
        int componentCount = 0;

        for (auto& pair : graph) {
            int nodeId = pair.first;
            if (!visited.count(nodeId)) {
                componentCount++;
                dfsComponent(nodeId, visited);
            }
        }

        return componentCount;
    }

    void dfsComponent(int node, unordered_set<int>& visited) {
        visited.insert(node);

        for (Edge* edge : graph[node]->edges) {
            int neighbor = (edge->node1 == node) ? edge->node2 : edge->node1;
            if (!visited.count(neighbor)) {
                dfsComponent(neighbor, visited);
            }
        }
    }

    // Нахождение всех компонент связности с помощью DFS
    vector<vector<int>> findConnectedComponents() {
        vector<vector<int>> components;
        unordered_set<int> visited;

        for (auto& pair : graph) {
            int nodeId = pair.first;
            if (!visited.count(nodeId)) {
                vector<int> component;
                dfsComponentAnalysis(nodeId, visited, component);
                components.push_back(component);
            }
        }

        return components;
    }

    void dfsComponentAnalysis(int node, unordered_set<int>& visited, vector<int>& component) {
        visited.insert(node);
        component.push_back(node);

        for (Edge* edge : graph[node]->edges) {
            int neighbor = (edge->node1 == node) ? edge->node2 : edge->node1;
            if (!visited.count(neighbor)) {
                dfsComponentAnalysis(neighbor, visited, component);
            }
        }
    }

    // Полный анализ графа
    void analyzeGraph() {
        cout << "=== АНАЛИЗ ГРАФА ===" << endl;

        int n = graph.size();
        int m = countEdges();
        int k = countConnectedComponents();

        cout << "ОСНОВНАЯ ИНФОРМАЦИЯ:" << endl;
        cout << "  Вершин: " << n << endl;
        cout << "  Ребер: " << m << endl;
        cout << "  Компонент связности: " << k << endl;

        cout << "\nПРОВЕРКА НА ЛЕС:" << endl;
        checkIsForest();

        cout << "\nПОИСК ЦИКЛОВ:" << endl;
        vector<vector<int>> cycles = findAllCycles();
        if (cycles.empty()) {
            cout << "  Циклов не найдено" << endl;
        }
        else {
            cout << "  Найдено циклов: " << cycles.size() << endl;
            for (int i = 0; i < cycles.size(); i++) {
                cout << "  Цикл " << (i + 1) << " (длина " << cycles[i].size() << "): ";
                for (int j = 0; j < cycles[i].size(); j++) {
                    cout << cycles[i][j];
                    if (j < cycles[i].size() - 1) cout << " -> ";
                }
                cout << endl;
            }
        }

        cout << "\nАНАЛИЗ КОМПОНЕНТ СВЯЗНОСТИ:" << endl;
        vector<vector<int>> components = findConnectedComponents();
        cout << "  Найдено компонент: " << components.size() << endl;

        for (int i = 0; i < components.size(); i++) {
            auto& component = components[i];
            int edgesInComponent = countEdgesInComponent(component);
            int isTree = (edgesInComponent == component.size() - 1);

            cout << "  Компонента " << (i + 1) << ":" << endl;
            cout << "    Вершины: ";
            for (int node : component) {
                cout << node << " ";
            }
            cout << endl;
            cout << "    Вершин: " << component.size() << ", Ребер: " << edgesInComponent << endl;
            cout << "    Тип: " << (isTree ? "Дерево" : "Не дерево") << endl;
            cout << endl;
        }

        cout << "=================================" << endl;
    }

    int countEdgesInComponent(const vector<int>& component) {
        unordered_set<int> compSet(component.begin(), component.end());
        int edges = 0;

        for (int node : component) {
            for (Edge* edge : graph[node]->edges) {
                int neighbor = (edge->node1 == node) ? edge->node2 : edge->node1;
                if (compSet.count(neighbor)) {
                    edges++;
                }
            }
        }
        return edges / 2;
    }
};

// Функция для очистки памяти
void cleanupGraph(unordered_map<int, Node*>& graph) {
    for (auto& pair : graph) {
        for (Edge* edge : pair.second->edges) {
            delete edge;
        }
        delete pair.second;
    }
    graph.clear();
}

// ТЕСТОВЫЕ ГРАФЫ

// 1. Пустой граф
unordered_map<int, Node*> createEmptyGraph() {
    return unordered_map<int, Node*>();
}

// 2. Лес (основной тестовый граф)
unordered_map<int, Node*> createTestForest() {
    unordered_map<int, Node*> graph;

    for (int i = 1; i <= 11; i++) {
        graph[i] = new Node(i);
    }

    // ДЕРЕВО 1: 1-2-3-4-5-6-7
    graph[1]->edges.push_back(new Edge(1, 3));
    graph[3]->edges.push_back(new Edge(3, 1));
    graph[3]->edges.push_back(new Edge(3, 5));
    graph[5]->edges.push_back(new Edge(5, 3));
    graph[3]->edges.push_back(new Edge(3, 4));
    graph[4]->edges.push_back(new Edge(4, 3));
    graph[1]->edges.push_back(new Edge(1, 2));
    graph[2]->edges.push_back(new Edge(2, 1));
    graph[2]->edges.push_back(new Edge(2, 6));
    graph[6]->edges.push_back(new Edge(6, 2));
    graph[6]->edges.push_back(new Edge(6, 7));
    graph[7]->edges.push_back(new Edge(7, 6));

    // ДЕРЕВО 2: 8-9-10
    graph[10]->edges.push_back(new Edge(10, 9));
    graph[9]->edges.push_back(new Edge(9, 10));
    graph[9]->edges.push_back(new Edge(9, 8));
    graph[8]->edges.push_back(new Edge(8, 9));

    return graph;
}

// 3. Граф с циклом
unordered_map<int, Node*> createGraphWithCycle() {
    unordered_map<int, Node*> graph;

    for (int i = 1; i <= 5; i++) {
        graph[i] = new Node(i);
    }

    // Создаем цикл 1-2-3-4-1
    graph[1]->edges.push_back(new Edge(1, 2));
    graph[2]->edges.push_back(new Edge(2, 1));
    graph[2]->edges.push_back(new Edge(2, 3));
    graph[3]->edges.push_back(new Edge(3, 2));
    graph[3]->edges.push_back(new Edge(3, 4));
    graph[4]->edges.push_back(new Edge(4, 3));
    graph[4]->edges.push_back(new Edge(4, 1)); // Замыкаем цикл
    graph[1]->edges.push_back(new Edge(1, 4));

    return graph;
}

// ТЕСТИРОВАНИЕ
void testGraph(const string& testName, unordered_map<int, Node*>& graph) {
    cout << "=================================" << endl;
    cout << "ТЕСТ: " << testName << endl;
    cout << "=================================" << endl;

    if (graph.empty()) {
        cout << "Граф пустой" << endl << endl;
        return;
    }

    ForestChecker checker(graph);
    checker.analyzeGraph();
    cout << endl;
}

int main() {
    setlocale(LC_ALL, "Russian");

    cout << "ТЕСТИРОВАНИЕ ПРОВЕРКИ НА ЛЕС И ПОИСКА ЦИКЛОВ" << endl;
    cout << "============================================" << endl << endl;

    // Тест 1: Пустой граф
    auto graph1 = createEmptyGraph();
    testGraph("Пустой граф", graph1);
    cleanupGraph(graph1);

    // Тест 2: Лес
    auto graph2 = createTestForest();
    testGraph("Лес", graph2);
    cleanupGraph(graph2);

    // Тест 3: Граф с циклом
    auto graph3 = createGraphWithCycle();
    testGraph("Граф с циклом", graph3);
    cleanupGraph(graph3);

    cout << "=================================" << endl;
    cout << "ВСЕ ТЕСТЫ ЗАВЕРШЕНЫ!" << endl;
    cout << "=================================" << endl;

    return 0;
}