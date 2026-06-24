#ifndef BITMASK_OPTIMIZER_H
#define BITMASK_OPTIMIZER_H

#include "Graph.h"
#include <cstdint>
#include <intrin.h>

#define MAX_NODES 40 // Оптимальный лимит под сетку 6х6 (36 узлов)

inline int count_active(uint64_t mask) { return (int)__popcnt64(mask); }

inline int get_lowest_bit(uint64_t mask) {
    unsigned long index;
    _BitScanForward64(&index, mask);
    return (int)index;
}

struct GraphMask {
    int num_nodes;
    uint64_t active_nodes;
    uint64_t adj[MAX_NODES];

    // Сжимаем в 1D массив. Для 40 узлов это всего 40*39/2 = 780 элементов.
    
    double prob[800];

    GraphMask(int n) : num_nodes(n), active_nodes((1ULL << n) - 1) {
        for (int i = 0; i < n; ++i) adj[i] = 0;
        for (int i = 0; i < 800; ++i) prob[i] = 0.0;
    }

    // Быстрое безветвистое (branchless) вычисление индекса в одномерном массиве
    inline int edge_idx(int u, int v) const {
        int mx = (u > v) ? u : v;
        int mn = (u > v) ? v : u;
        return mx * (mx - 1) / 2 + mn;
    }

    inline double& p(int u, int v) {
        return prob[edge_idx(u, v)];
    }

    inline double p_const(int u, int v) const {
        return prob[edge_idx(u, v)];
    }

    void add_edge(int u, int v, double p_val) {
        adj[u] |= (1ULL << v);
        adj[v] |= (1ULL << u);
        p(u, v) = p_val;
    }
};

inline bool is_connected_mask(const GraphMask& g) {
    if (g.active_nodes == 0) return true;
    uint64_t visited = g.active_nodes & -(long long)g.active_nodes;
    uint64_t front = visited;

    while (front) {
        uint64_t next_front = 0;
        uint64_t temp = front;
        while (temp) {
            int u = get_lowest_bit(temp);
            next_front |= g.adj[u];
            temp &= temp - 1;
        }
        next_front &= ~visited;
        next_front &= g.active_nodes;
        visited |= next_front;
        front = next_front;
    }
    return visited == g.active_nodes;
}

inline void delete_edge_mask(GraphMask& g, int u, int v) {
    g.adj[u] &= ~(1ULL << v);
    g.adj[v] &= ~(1ULL << u);
}

inline void contract_edge_mask(GraphMask& g, int u, int v) {
    uint64_t v_neighbors = g.adj[v] & ~(1ULL << u);
    while (v_neighbors) {
        int neighbor = get_lowest_bit(v_neighbors);
        double p_v = g.p_const(v, neighbor);

        if (g.adj[u] & (1ULL << neighbor)) {
            double p_u = g.p_const(u, neighbor);
            double p_merged = 1.0 - (1.0 - p_u) * (1.0 - p_v);
            g.p(u, neighbor) = p_merged;
        }
        else {
            g.adj[u] |= (1ULL << neighbor);
            g.adj[neighbor] |= (1ULL << u);
            g.p(u, neighbor) = p_v;
        }
        g.adj[neighbor] &= ~(1ULL << v);
        v_neighbors &= ~(1ULL << neighbor);
    }
    g.adj[v] = 0;
    g.active_nodes &= ~(1ULL << v);
    g.adj[u] &= ~(1ULL << v);
}

inline void simplify_mask(GraphMask& g, double& mult) {
    bool changed = true;
    while (changed) {
        changed = false;
        uint64_t active = g.active_nodes;
        while (active) {
            int i = get_lowest_bit(active);
            active &= active - 1;

            int deg = count_active(g.adj[i]);
            if (deg == 1) {
                int neighbor = get_lowest_bit(g.adj[i]);
                mult *= g.p_const(i, neighbor);
                g.adj[neighbor] &= ~(1ULL << i);
                g.adj[i] = 0;
                g.active_nodes &= ~(1ULL << i);
                changed = true;
            }
            else if (deg == 2) {
                int n1 = get_lowest_bit(g.adj[i]);
                int n2 = get_lowest_bit(g.adj[i] & ~(1ULL << n1));

                double p1 = g.p_const(i, n1);
                double p2 = g.p_const(i, n2);
                double denom = p1 + p2 - p1 * p2;
                double p_new = (denom > 0.0) ? (p1 * p2) / denom : 0.0;
                mult *= denom;

                g.adj[n1] &= ~(1ULL << i);
                g.adj[n2] &= ~(1ULL << i);
                g.adj[i] = 0;
                g.active_nodes &= ~(1ULL << i);

                if (g.adj[n1] & (1ULL << n2)) {
                    double p_old = g.p_const(n1, n2);
                    g.p(n1, n2) = 1.0 - (1.0 - p_old) * (1.0 - p_new);
                }
                else {
                    g.adj[n1] |= (1ULL << n2);
                    g.adj[n2] |= (1ULL << n1);
                    g.p(n1, n2) = p_new;
                }
                changed = true;
            }
        }
    }
}

inline void choose_edge_mask(const GraphMask& g, int& out_u, int& out_v) {
    int min_deg_sum = 100000;
    out_u = -1; out_v = -1;
    uint64_t active = g.active_nodes;

    while (active) {
        int u = get_lowest_bit(active);
        active &= active - 1;

        int deg_u = count_active(g.adj[u]);
        uint64_t neighbors = g.adj[u];

        while (neighbors) {
            int v = get_lowest_bit(neighbors);
            neighbors &= neighbors - 1;

            if (u < v) {
                int deg_sum = deg_u + count_active(g.adj[v]);
                if (deg_sum == 6 && (g.adj[u] & g.adj[v])) {
                    out_u = u; out_v = v;
                    return;
                }
                if (deg_sum < min_deg_sum) {
                    min_deg_sum = deg_sum;
                    out_u = u; out_v = v;
                }
            }
        }
    }
}

inline double factoring_mask_rec(const GraphMask& g_in, bool assumed_connected) {
    GraphMask g = g_in;

    if (count_active(g.active_nodes) <= 1) return 1.0;

    // ОПТИМИЗАЦИЯ: Пропускаем проверку связности, если пришли из ветки стягивания
    if (!assumed_connected && !is_connected_mask(g)) return 0.0;

    double mult = 1.0;
    simplify_mask(g, mult);

    int n_active = count_active(g.active_nodes);
    if (n_active <= 1) return mult * 1.0;
    if (!assumed_connected && !is_connected_mask(g)) return 0.0;

    if (n_active == 2) {
        int u = get_lowest_bit(g.active_nodes);
        int v = get_lowest_bit(g.active_nodes & ~(1ULL << u));
        return mult * g.p_const(u, v);
    }

    if (n_active == 3) {
        int u = get_lowest_bit(g.active_nodes);
        int v = get_lowest_bit(g.active_nodes & ~(1ULL << u));
        int w = get_lowest_bit(g.active_nodes & ~((1ULL << u) | (1ULL << v)));

        double p1 = (g.adj[u] & (1ULL << v)) ? g.p_const(u, v) : 0.0;
        double p2 = (g.adj[v] & (1ULL << w)) ? g.p_const(v, w) : 0.0;
        double p3 = (g.adj[u] & (1ULL << w)) ? g.p_const(u, w) : 0.0;

        return mult * (p1 * p2 + p1 * p3 + p2 * p3 - 2.0 * p1 * p2 * p3);
    }

    int u, v;
    choose_edge_mask(g, u, v);
    if (u == -1 || v == -1) return 0.0;

    double p = g.p_const(u, v);

    GraphMask g_contract = g;
    contract_edge_mask(g_contract, u, v);
    // Стягивание сохраняет связность! Передаем true
    double rel_contract = factoring_mask_rec(g_contract, true);

    delete_edge_mask(g, u, v);
    // Удаление может разорвать связность. Передаем false
    double rel_delete = factoring_mask_rec(g, false);

    return mult * (p * rel_contract + (1.0 - p) * rel_delete);
}

inline double factoring_mask(const GraphMask& g) {
    return factoring_mask_rec(g, false); // Стартуем с обязательной проверки
}

inline GraphMask convertToMask(math::Graph* g) {
    int n = g->getVertNumb();
    if (n > MAX_NODES) n = MAX_NODES;

    GraphMask mask(n);
    u_int* KAO = g->getKAO();
    u_int* FO = g->getFO();

    for (int i = 1; i <= n; ++i) {
        for (u_int j = KAO[i - 1]; j < KAO[i]; ++j) {
            int u = i - 1;
            int v = FO[j] - 1;
            if (v < MAX_NODES) mask.add_edge(u, v, 0.75);
        }
    }
    return mask;
}
#endif