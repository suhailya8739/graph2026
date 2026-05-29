/**
 * @file stoer_wagner_min_cut_test.cpp
 * @author Ilya Sukharev
 *
 * Тесты для алгоритма graph::StoerWagnerMinCut.
 */

#include <httplib.h>
#include <string>
#include <vector>
#include <random>
#include <nlohmann/json.hpp>
#include "test_core.hpp"

static void SimpleTestTwoVertices(httplib::Client* cli);
static void SimpleTestFourVertices(httplib::Client* cli);
static void RandomTest(httplib::Client* cli);

void TestStoerWagnerMinCut(httplib::Client* cli) {
    TestSuite suite("TestStoerWagnerMinCut");

    RUN_TEST_REMOTE(suite, cli, SimpleTestTwoVertices);
    RUN_TEST_REMOTE(suite, cli, SimpleTestFourVertices);
    RUN_TEST_REMOTE(suite, cli, RandomTest);
}

/**
 * @brief Простейший статический тест для графа из двух вершин.
 *
 * @param cli Указатель на HTTP клиент.
 *
 * Граф: вершины 0 и 1, одно ребро весом 5.
 * Минимальный разрез равен 5.
 */
static void SimpleTestTwoVertices(httplib::Client* cli) {
    nlohmann::json input = R"(
{
  "graph_type": "WeightedGraph",
  "weight_type": "int",
  "vertices": [0, 1],
  "edges": [
    { "from": 0, "to": 1, "weight": 5 }
  ]
}
)"_json;

    auto res = cli->Post("/StoerWagnerMinCut", input.dump(),
        "application/json");

    if (!res) {
        REQUIRE(false);
    }

    nlohmann::json output = nlohmann::json::parse(res->body);

    int result = output.at("result");

    REQUIRE_EQUAL(5, result);
}

/**
 * @brief Статический тест для графа из четырёх вершин.
 *
 * @param cli Указатель на HTTP клиент.
 *
 * Граф: цикл 0-1-2-3-0 с весами 2, 1, 4, 3.
 * Минимальный разрез равен 3 (прямая проверка перебором: 
 * оптимальный вариант, когда отрезаем вершину 1 ребра (0,1)=2 и (1,2)=1, итого 3).
 */
static void SimpleTestFourVertices(httplib::Client* cli) {
    nlohmann::json input;

    input["graph_type"] = "WeightedGraph";
    input["weight_type"] = "int";
    input["vertices"] = std::vector<int>{ 0, 1, 2, 3 };

    input["edges"][0]["from"] = 0;
    input["edges"][0]["to"] = 1;
    input["edges"][0]["weight"] = 2;

    input["edges"][1]["from"] = 1;
    input["edges"][1]["to"] = 2;
    input["edges"][1]["weight"] = 1;

    input["edges"][2]["from"] = 2;
    input["edges"][2]["to"] = 3;
    input["edges"][2]["weight"] = 4;

    input["edges"][3]["from"] = 3;
    input["edges"][3]["to"] = 0;
    input["edges"][3]["weight"] = 3;

    auto res = cli->Post("/StoerWagnerMinCut", input.dump(),
        "application/json");

    if (!res) {
        REQUIRE(false);
    }

    nlohmann::json output = nlohmann::json::parse(res->body);

    int result = output.at("result");

    REQUIRE_EQUAL(3, result);
}

/**
 * @brief Случайный тест.
 *
 * @param cli Указатель на HTTP клиент.
 *
 * Генерируем случайный полный граф на n вершинах с целыми весами.
 * Проверяем, что результат не превышает минимальную взвешенную степень
 * вершины: разрез, отделяющий одну вершину, равен сумме ее ребер,
 * поэтому глобальный минимальный разрез не может быть больше.
 */
static void RandomTest(httplib::Client* cli) {
    // Число попыток.
    const int numTries = 10;
    // Используется для инициализации генератора случайных чисел.
    std::random_device rd;
    // Генератор случайных чисел.
    std::mt19937 gen(rd());
    // Распределение для весов рёбер.
    std::uniform_int_distribution<int> weightDist(1, 10);
    // Распределение для числа вершин.
    std::uniform_int_distribution<size_t> sizeDist(2, 8);

    for (int it = 0; it < numTries; it++) {
        size_t n = sizeDist(gen);

        nlohmann::json input;
        input["graph_type"] = "WeightedGraph";
        input["weight_type"] = "int";

        std::vector<int> vertices;
        for (size_t i = 0; i < n; i++)
            vertices.push_back(static_cast<int>(i));

        input["vertices"] = vertices;

        /* Строим полный граф со случайными весами и считаем степени вершин. */
        std::vector<int> degree(n, 0);
        size_t edgeIdx = 0;
        for (size_t i = 0; i < n; i++) {
            for (size_t j = i + 1; j < n; j++) {
                int weight = weightDist(gen);
                input["edges"][edgeIdx]["from"] = i;
                input["edges"][edgeIdx]["to"] = j;
                input["edges"][edgeIdx]["weight"] = weight;
                edgeIdx++;
                degree[i] += weight;
                degree[j] += weight;
            }
        }

        auto res = cli->Post("/StoerWagnerMinCut", input.dump(),
            "application/json");

        if (!res) {
            REQUIRE(false);
        }

        nlohmann::json output = nlohmann::json::parse(res->body);

        int result = output.at("result");

        int minDegree = degree[0];
        for (size_t i = 1; i < n; i++) {
            if (degree[i] < minDegree)
                minDegree = degree[i];
        }

        REQUIRE(result <= minDegree);
        REQUIRE(result > 0);
    }
}
