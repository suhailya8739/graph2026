/**
 * @file include/stoer_wagner_min_cut.hpp
 * @author Ilya Sukharev
 *
 * Алгоритм нахождения минимального разреза (алгоритм Штора-Вагнера).
 */

#ifndef INCLUDE_STOER_WAGNER_MIN_CUT_HPP_
#define INCLUDE_STOER_WAGNER_MIN_CUT_HPP_

#include <vector>
#include <unordered_map>
#include <limits>
#include <cstddef>

namespace graph {

    /**
     * @brief Алгоритм нахождения минимального разреза (алгоритм Штора-Вагнера).
     *
     * @tparam GraphType Тип графа. Граф должен быть взвешенным неориентированным
     * и предоставлять методы Vertices(), NumVertices(), Edges() и EdgeWeight().
     * @param graph Граф для нахождения минимального разреза.
     * @return Значение минимального разреза. Если граф содержит менее двух вершин,
     * возвращается значение, инициализированное конструктором по умолчанию.
     *
     * Функция реализует алгоритм Штора-Вагнера для нахождения глобального
     * минимального разреза во взвешенном неориентированном графе с
     * неотрицательными весами рёбер.
     */
template<typename GraphType>
typename GraphType::WeightType StoerWagnerMinCut(const GraphType& graph) {
        using Weight = typename GraphType::WeightType;

        size_t n = graph.NumVertices();
        if (n < 2)
            return Weight();

        std::vector<size_t> vertices;
        vertices.reserve(n);
        std::unordered_map<size_t, size_t> vidx;
        for (size_t v : graph.Vertices()) {
            vidx[v] = vertices.size();
            vertices.push_back(v);
        }

        std::vector<std::vector<Weight>> g(n, std::vector<Weight>(n, Weight()));
        for (size_t i = 0; i < n; i++) {
            for (size_t nb : graph.Edges(vertices[i])) {
                size_t j = vidx.at(nb);
                g[i][j] = graph.EdgeWeight(vertices[i], nb);
            }
        }

        std::vector<bool> exist(n, true);

        Weight best = std::numeric_limits<Weight>::max();

        /* Подалгоритм: находим n-1 раз минимальный разрез между вершинами. */
        for (size_t ph = 0; ph < n - 1; ph++) {
            std::vector<Weight> w(n, Weight());
            std::vector<bool> in_a(n, false);

            size_t prev = 0;
            for (size_t it = 0; it < n - ph; it++) {
                size_t sel = n;
                for (size_t i = 0; i < n; i++) {
                    if (exist[i] && !in_a[i] && (sel == n || w[i] > w[sel]))
                        sel = i;
                }

                if (it == n - ph - 1) {
                    if (w[sel] < best)
                        best = w[sel];

                    for (size_t i = 0; i < n; i++)
                        g[prev][i] = g[i][prev] += g[sel][i];

                    exist[sel] = false;
                } else {
                    in_a[sel] = true;

                    for (size_t i = 0; i < n; i++)
                        w[i] += g[sel][i];

                    prev = sel;
                }
            }
        }

      return best;
}

}  // namespace graph

#endif  // INCLUDE_STOER_WAGNER_MIN_CUT_HPP_

