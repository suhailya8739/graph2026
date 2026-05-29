#include <string>
#include <nlohmann/json.hpp>
#include <graph.hpp>
#include <weighted_graph.hpp>
#include <stoer_wagner_min_cut.hpp>

using graph::WeightedGraph;

namespace graph {

template<typename GraphType>
int StoerWagnerMinCutMethodHelper(const nlohmann::json& input,
    nlohmann::json* output);

int StoerWagnerMinCutMethod(const nlohmann::json& input,
    nlohmann::json* output) {
  std::string graphType = input.at("graph_type");

  if (graphType == "WeightedGraph") {
    std::string weightType = input.at("weight_type");
    if (weightType == "int") {
      return StoerWagnerMinCutMethodHelper<WeightedGraph<int>>(input, output);
    } else if (weightType == "double") {
      return StoerWagnerMinCutMethodHelper<WeightedGraph<double>>(
          input, output);
    } else {
      return -1;
    }
  }

  return -1;
}

/**
 * @brief Метод нахождения минимального разреза (алгоритм Штора-Вагнера).
 *
 * @param input Входные данные в формате JSON.
 * @param output Выходные данные в формате JSON.
 * @return Функция возвращает 0 в случае успеха и отрицательное число
 * если входные данные заданы некорректно.
 *
 * Функция запускает алгоритм Штора-Вагнера, используя входные данные
 * в JSON формате. Результат также выдаётся в JSON формате.
 */
template<typename GraphType>
int StoerWagnerMinCutMethodHelper(const nlohmann::json& input,
    nlohmann::json* output) {
  GraphType graph;

  for (auto& vertex : input.at("vertices")) {
    graph.AddVertex(vertex);
  }

  for (auto& edge : input.at("edges")) {
    graph.AddEdge(edge.at("from"), edge.at("to"), edge.at("weight"));
  }

  (*output)["result"] = StoerWagnerMinCut(graph);

  return 0;
}

}  // namespace graph
