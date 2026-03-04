#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
#include <clocale>
#include <functional>

class Node;
class Edge;

struct Node
{
    int id;
    std::vector<Edge*> edges;
    std::unordered_map<Node*, Edge*> parents;

    Node(int nodeId) : id(nodeId) {}
};

struct Edge
{
    int weight;
    Node* adjacentNode;

    Edge(int w, Node* node) : weight(w), adjacentNode(node) {}
};

// Структура для остаточного ребра
struct ResidualEdge {
    int capacity;     // остаточная пропускная способность
    Edge* originalEdge; // ссылка на оригинальное ребро
    ResidualEdge* reverse; // указатель на обратное ребро

    ResidualEdge(int cap = 0, Edge* orig = nullptr, ResidualEdge* rev = nullptr)
        : capacity(cap), originalEdge(orig), reverse(rev) {
    }
};

// Вспомогательная функция для подсчета ребер
int countEdges(std::unordered_map<int, Node*>& graph) {
    int edgeCount = 0;
    for (auto& pair : graph) {
        edgeCount += pair.second->edges.size();
    }
    return edgeCount;
}

// Алгоритм Диница
int dinic(std::unordered_map<int, Node*>& graph, int sourceId, int sinkId) {
    // Вывод характеристик графа как у коллеги
    int vertexCount = graph.size();
    int edgeCount = countEdges(graph);

    std::cout << "Характеристики графа:" << std::endl;
    std::cout << "    Вершин: " << vertexCount << std::endl;
    std::cout << "    Ребер: " << edgeCount << std::endl;
    std::cout << "Источник: " << sourceId << ", Сток: " << sinkId << std::endl;

    // Проверка входных данных
    if (graph.empty()) {
        std::cout << "Граф пустой" << std::endl;
        std::cout << "Результат алгоритма: 0" << std::endl << std::endl;
        return 0;
    }

    if (graph.find(sourceId) == graph.end()) {
        std::cout << "Вершина-источник " << sourceId << " не найдена" << std::endl;
        std::cout << "Результат алгоритма: 0" << std::endl << std::endl;
        return 0;
    }

    if (graph.find(sinkId) == graph.end()) {
        std::cout << "Вершина-сток " << sinkId << " не найдена" << std::endl;
        std::cout << "Результат алгоритма: 0" << std::endl << std::endl;
        return 0;
    }

    if (sourceId == sinkId) {
        std::cout << "Источник и сток – одна и та же вершина" << std::endl;
        std::cout << "Результат алгоритма: 0" << std::endl << std::endl;
        return 0;
    }

    Node* source = graph[sourceId];
    Node* sink = graph[sinkId];

    // 1. Создаём остаточную сеть
    std::unordered_map<Node*, std::vector<ResidualEdge*>> residualGraph;

    // Инициализируем для всех вершин
    for (auto& pair : graph) {
        residualGraph[pair.second] = std::vector<ResidualEdge*>();
    }

    // Заполняем остаточную сеть
    for (auto& pair : graph) {
        Node* u = pair.second;

        for (Edge* edge : u->edges) {
            Node* v = edge->adjacentNode;

            // Прямое и обратное ребро
            ResidualEdge* forward = new ResidualEdge(edge->weight, edge);
            ResidualEdge* backward = new ResidualEdge(0, nullptr);

            // Связываем их
            forward->reverse = backward;
            backward->reverse = forward;

            // Добавляем в остаточную сеть
            residualGraph[u].push_back(forward);
            residualGraph[v].push_back(backward);
        }
    }

    int maxFlow = 0;

    // Для уровней
    std::unordered_map<Node*, int> level;

    // Для указателей
    std::unordered_map<Node*, int> ptr;

    // 2. BFS для построения слоистой сети
    auto bfs = [&]() -> bool {
        // Очищаем уровни
        for (auto& pair : graph) {
            level[pair.second] = -1;
        }

        std::queue<Node*> q;
        q.push(source);
        level[source] = 0;

        while (!q.empty()) {
            Node* u = q.front();
            q.pop();

            for (ResidualEdge* re : residualGraph[u]) {
                // Находим вершину назначения
                Node* v = nullptr;
                if (re->originalEdge) {
                    v = re->originalEdge->adjacentNode;
                }
                else {
                    // Для обратного ребра
                    for (auto& pair2 : graph) {
                        bool found = false;
                        for (ResidualEdge* re2 : residualGraph[pair2.second]) {
                            if (re2 == re->reverse) {
                                v = pair2.second;
                                found = true;
                                break;
                            }
                        }
                        if (found) break;
                    }
                }

                if (v && re->capacity > 0 && level[v] == -1) {
                    level[v] = level[u] + 1;
                    q.push(v);
                }
            }
        }

        return level[sink] != -1;
        };

    // 3. DFS для поиска блокирующего потока
    std::function<int(Node*, int)> dfs = [&](Node* u, int flow) -> int {
        if (u == sink) {
            return flow;
        }

        for (int& i = ptr[u]; i < residualGraph[u].size(); ++i) {
            ResidualEdge* re = residualGraph[u][i];

            Node* v = nullptr;
            if (re->originalEdge) {
                v = re->originalEdge->adjacentNode;
            }
            else {
                for (auto& pair2 : graph) {
                    bool found = false;
                    for (ResidualEdge* re2 : residualGraph[pair2.second]) {
                        if (re2 == re->reverse) {
                            v = pair2.second;
                            found = true;
                            break;
                        }
                    }
                    if (found) break;
                }
            }

            if (v && level[v] == level[u] + 1 && re->capacity > 0) {
                int pushed = dfs(v, std::min(flow, re->capacity));
                if (pushed > 0) {
                    re->capacity -= pushed;
                    re->reverse->capacity += pushed;
                    return pushed;
                }
            }
        }

        return 0;
        };

    // 4. Основной цикл алгоритма Диница
    while (bfs()) {
        // Сбрасываем указатели
        for (auto& pair : graph) {
            ptr[pair.second] = 0;
        }

        int pushed;
        while ((pushed = dfs(source, INT_MAX)) > 0) {
            maxFlow += pushed;
        }
    }

    // 5. Очищаем память
    for (auto& pair : residualGraph) {
        for (ResidualEdge* re : pair.second) {
            delete re;
        }
    }

    std::cout << "Результат алгоритма: " << maxFlow << std::endl << std::endl;
    return maxFlow;
}


// 1. ПУСТОЙ ГРАФ (без ребер)
void createEmptyGraph(std::unordered_map<int, Node*>& graph) {
    for (int i = 1; i <= 3; i++) {
        graph[i] = new Node(i);
    }
}

// 2. ГРАФ С 3 ВЕРШИНАМИ
void createGraph3Vertices(std::unordered_map<int, Node*>& graph) {
    for (int i = 1; i <= 3; i++) {
        graph[i] = new Node(i);
    }

    graph[1]->edges.push_back(new Edge(10, graph[2]));
    graph[2]->parents[graph[1]] = graph[1]->edges.back();

    graph[2]->edges.push_back(new Edge(5, graph[3]));
    graph[3]->parents[graph[2]] = graph[2]->edges.back();
}

// 3. ТЕСТОВЫЙ ГРАФ ИЗ ЗАДАНИЯ
void createTestGraph(std::unordered_map<int, Node*>& graph) {
    for (int i = 1; i <= 5; i++) {
        graph[i] = new Node(i);
    }

    graph[1]->edges.push_back(new Edge(40, graph[3]));
    graph[3]->parents[graph[1]] = graph[1]->edges.back();

    graph[1]->edges.push_back(new Edge(30, graph[2]));
    graph[2]->parents[graph[1]] = graph[1]->edges.back();

    graph[1]->edges.push_back(new Edge(20, graph[4]));
    graph[4]->parents[graph[1]] = graph[1]->edges.back();

    graph[2]->edges.push_back(new Edge(50, graph[3]));
    graph[3]->parents[graph[2]] = graph[2]->edges.back();

    graph[3]->edges.push_back(new Edge(20, graph[4]));
    graph[4]->parents[graph[3]] = graph[3]->edges.back();

    graph[4]->edges.push_back(new Edge(30, graph[5]));
    graph[5]->parents[graph[4]] = graph[4]->edges.back();

    graph[3]->edges.push_back(new Edge(30, graph[5]));
    graph[5]->parents[graph[3]] = graph[3]->edges.back();

    graph[2]->edges.push_back(new Edge(40, graph[5]));
    graph[5]->parents[graph[2]] = graph[2]->edges.back();
}

// 4. ГРАФ С 6 ВЕРШИНАМИ (классический пример)
void createGraph6Vertices(std::unordered_map<int, Node*>& graph) {
    for (int i = 1; i <= 6; i++) {
        graph[i] = new Node(i);
    }

    graph[1]->edges.push_back(new Edge(16, graph[2]));
    graph[2]->parents[graph[1]] = graph[1]->edges.back();

    graph[1]->edges.push_back(new Edge(13, graph[3]));
    graph[3]->parents[graph[1]] = graph[1]->edges.back();

    graph[2]->edges.push_back(new Edge(12, graph[3]));
    graph[3]->parents[graph[2]] = graph[2]->edges.back();

    graph[2]->edges.push_back(new Edge(10, graph[4]));
    graph[4]->parents[graph[2]] = graph[2]->edges.back();

    graph[3]->edges.push_back(new Edge(9, graph[2]));
    graph[2]->parents[graph[3]] = graph[3]->edges.back();

    graph[3]->edges.push_back(new Edge(14, graph[5]));
    graph[5]->parents[graph[3]] = graph[3]->edges.back();

    graph[4]->edges.push_back(new Edge(7, graph[5]));
    graph[5]->parents[graph[4]] = graph[4]->edges.back();

    graph[4]->edges.push_back(new Edge(4, graph[6]));
    graph[6]->parents[graph[4]] = graph[4]->edges.back();

    graph[5]->edges.push_back(new Edge(20, graph[6]));
    graph[6]->parents[graph[5]] = graph[5]->edges.back();
}

// 5. ГРАФ С 10 ВЕРШИНАМИ
void createGraph10Vertices(std::unordered_map<int, Node*>& graph) {
    for (int i = 1; i <= 10; i++) {
        graph[i] = new Node(i);
    }

    graph[1]->edges.push_back(new Edge(20, graph[2]));
    graph[2]->parents[graph[1]] = graph[1]->edges.back();

    graph[1]->edges.push_back(new Edge(15, graph[3]));
    graph[3]->parents[graph[1]] = graph[1]->edges.back();

    graph[1]->edges.push_back(new Edge(10, graph[4]));
    graph[4]->parents[graph[1]] = graph[1]->edges.back();

    graph[2]->edges.push_back(new Edge(25, graph[5]));
    graph[5]->parents[graph[2]] = graph[2]->edges.back();

    graph[3]->edges.push_back(new Edge(10, graph[5]));
    graph[5]->parents[graph[3]] = graph[3]->edges.back();

    graph[3]->edges.push_back(new Edge(15, graph[6]));
    graph[6]->parents[graph[3]] = graph[3]->edges.back();

    graph[4]->edges.push_back(new Edge(20, graph[6]));
    graph[6]->parents[graph[4]] = graph[4]->edges.back();

    graph[5]->edges.push_back(new Edge(15, graph[7]));
    graph[7]->parents[graph[5]] = graph[5]->edges.back();

    graph[5]->edges.push_back(new Edge(10, graph[8]));
    graph[8]->parents[graph[5]] = graph[5]->edges.back();

    graph[6]->edges.push_back(new Edge(20, graph[8]));
    graph[8]->parents[graph[6]] = graph[6]->edges.back();

    graph[6]->edges.push_back(new Edge(5, graph[9]));
    graph[9]->parents[graph[6]] = graph[6]->edges.back();

    graph[7]->edges.push_back(new Edge(30, graph[10]));
    graph[10]->parents[graph[7]] = graph[7]->edges.back();

    graph[8]->edges.push_back(new Edge(20, graph[10]));
    graph[10]->parents[graph[8]] = graph[8]->edges.back();

    graph[9]->edges.push_back(new Edge(10, graph[10]));
    graph[10]->parents[graph[9]] = graph[9]->edges.back();
}

// 6. ГРАФ С 15 ВЕРШИНАМИ
void createGraph15Vertices(std::unordered_map<int, Node*>& graph) {
    for (int i = 1; i <= 15; i++) {
        graph[i] = new Node(i);
    }

    graph[1]->edges.push_back(new Edge(50, graph[2]));
    graph[2]->parents[graph[1]] = graph[1]->edges.back();

    graph[1]->edges.push_back(new Edge(40, graph[3]));
    graph[3]->parents[graph[1]] = graph[1]->edges.back();

    graph[1]->edges.push_back(new Edge(30, graph[4]));
    graph[4]->parents[graph[1]] = graph[1]->edges.back();

    graph[1]->edges.push_back(new Edge(20, graph[5]));
    graph[5]->parents[graph[1]] = graph[1]->edges.back();

    graph[2]->edges.push_back(new Edge(15, graph[6]));
    graph[6]->parents[graph[2]] = graph[2]->edges.back();

    graph[2]->edges.push_back(new Edge(10, graph[7]));
    graph[7]->parents[graph[2]] = graph[2]->edges.back();

    graph[3]->edges.push_back(new Edge(20, graph[7]));
    graph[7]->parents[graph[3]] = graph[3]->edges.back();

    graph[3]->edges.push_back(new Edge(15, graph[8]));
    graph[8]->parents[graph[3]] = graph[3]->edges.back();

    graph[4]->edges.push_back(new Edge(25, graph[8]));
    graph[8]->parents[graph[4]] = graph[4]->edges.back();

    graph[4]->edges.push_back(new Edge(10, graph[9]));
    graph[9]->parents[graph[4]] = graph[4]->edges.back();

    graph[5]->edges.push_back(new Edge(30, graph[9]));
    graph[9]->parents[graph[5]] = graph[5]->edges.back();

    graph[5]->edges.push_back(new Edge(5, graph[10]));
    graph[10]->parents[graph[5]] = graph[5]->edges.back();

    graph[6]->edges.push_back(new Edge(40, graph[11]));
    graph[11]->parents[graph[6]] = graph[6]->edges.back();

    graph[7]->edges.push_back(new Edge(20, graph[11]));
    graph[11]->parents[graph[7]] = graph[7]->edges.back();

    graph[7]->edges.push_back(new Edge(15, graph[12]));
    graph[12]->parents[graph[7]] = graph[7]->edges.back();

    graph[8]->edges.push_back(new Edge(25, graph[12]));
    graph[12]->parents[graph[8]] = graph[8]->edges.back();

    graph[8]->edges.push_back(new Edge(10, graph[13]));
    graph[13]->parents[graph[8]] = graph[8]->edges.back();

    graph[9]->edges.push_back(new Edge(30, graph[13]));
    graph[13]->parents[graph[9]] = graph[9]->edges.back();

    graph[9]->edges.push_back(new Edge(5, graph[14]));
    graph[14]->parents[graph[9]] = graph[9]->edges.back();

    graph[10]->edges.push_back(new Edge(35, graph[14]));
    graph[14]->parents[graph[10]] = graph[10]->edges.back();

    graph[11]->edges.push_back(new Edge(50, graph[15]));
    graph[15]->parents[graph[11]] = graph[11]->edges.back();

    graph[12]->edges.push_back(new Edge(45, graph[15]));
    graph[15]->parents[graph[12]] = graph[12]->edges.back();

    graph[13]->edges.push_back(new Edge(35, graph[15]));
    graph[15]->parents[graph[13]] = graph[13]->edges.back();

    graph[14]->edges.push_back(new Edge(25, graph[15]));
    graph[15]->parents[graph[14]] = graph[14]->edges.back();

    graph[12]->edges.push_back(new Edge(5, graph[8]));
    graph[8]->parents[graph[12]] = graph[12]->edges.back();

    graph[14]->edges.push_back(new Edge(3, graph[10]));
    graph[10]->parents[graph[14]] = graph[14]->edges.back();
}

// 7. ГРАФ С НУЛЕВОЙ ПРОПУСКНОЙ СПОСОБНОСТЬЮ
void createZeroCapacityGraph(std::unordered_map<int, Node*>& graph) {
    for (int i = 1; i <= 4; i++) {
        graph[i] = new Node(i);
    }

    graph[1]->edges.push_back(new Edge(0, graph[2]));
    graph[2]->parents[graph[1]] = graph[1]->edges.back();

    graph[2]->edges.push_back(new Edge(0, graph[3]));
    graph[3]->parents[graph[2]] = graph[2]->edges.back();

    graph[3]->edges.push_back(new Edge(0, graph[4]));
    graph[4]->parents[graph[3]] = graph[3]->edges.back();
}

// Функция для очистки графа
void cleanupGraph(std::unordered_map<int, Node*>& graph) {
    for (auto& pair : graph) {
        for (Edge* edge : pair.second->edges) {
            delete edge;
        }
        delete pair.second;
    }
    graph.clear();
}

// ============ ОСНОВНАЯ ПРОГРАММА С ВСЕМИ ТЕСТАМИ ============
int main() {
    setlocale(LC_ALL, "");
    std::unordered_map<int, Node*> graph;

    std::cout << "============================================================================" << std::endl;
    std::cout << "ТЕСТ: ГРАФ С 3 ВЕРШИНАМИ" << std::endl;
    std::cout << "============================================================================" << std::endl;
    createGraph3Vertices(graph);
    dinic(graph, 1, 3);
    cleanupGraph(graph);

    std::cout << "============================================================================" << std::endl;
    std::cout << "ТЕСТ: ГРАФ С 6 ВЕРШИНАМИ (классический)" << std::endl;
    std::cout << "============================================================================" << std::endl;
    createGraph6Vertices(graph);
    dinic(graph, 1, 6);
    cleanupGraph(graph);

    std::cout << "============================================================================" << std::endl;
    std::cout << "ТЕСТ: ГРАФ С 10 ВЕРШИНАМИ" << std::endl;
    std::cout << "============================================================================" << std::endl;
    createGraph10Vertices(graph);
    dinic(graph, 1, 10);
    cleanupGraph(graph);

    std::cout << "============================================================================" << std::endl;
    std::cout << "ТЕСТ: ГРАФ С 15 ВЕРШИНАМИ" << std::endl;
    std::cout << "============================================================================" << std::endl;
    createGraph15Vertices(graph);
    dinic(graph, 1, 15);
    cleanupGraph(graph);

    std::cout << "============================================================================" << std::endl;
    std::cout << "ТЕСТ: ПУСТОЙ ГРАФ (без ребер)" << std::endl;
    std::cout << "============================================================================" << std::endl;
    createEmptyGraph(graph);
    dinic(graph, 1, 3);
    cleanupGraph(graph);

    std::cout << "============================================================================" << std::endl;
    std::cout << "ТЕСТ: ГРАФ С НУЛЕВОЙ ПРОПУСКНОЙ СПОСОБНОСТЬЮ" << std::endl;
    std::cout << "============================================================================" << std::endl;
    createZeroCapacityGraph(graph);
    dinic(graph, 1, 4);
    cleanupGraph(graph);

    std::cout << "============================================================================" << std::endl;
    std::cout << "ТЕСТ: ИСТОЧНИК И СТОК СОВПАДАЮТ" << std::endl;
    std::cout << "============================================================================" << std::endl;
    createGraph6Vertices(graph);
    dinic(graph, 1, 1);
    cleanupGraph(graph);

    std::cout << "============================================================================" << std::endl;
    std::cout << "ТЕСТ: НЕСУЩЕСТВУЮЩАЯ ВЕРШИНА" << std::endl;
    std::cout << "============================================================================" << std::endl;
    createGraph6Vertices(graph);
    dinic(graph, 1, 100);
    cleanupGraph(graph);

    return 0;
}