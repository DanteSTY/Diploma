#include <iostream>
#include <chrono>
#include <iomanip>
#include <vector>
#include <string>

#include "Graph.h"        
#include "GraphUtil.h"    
#include "BitmaskOptimizer.h" 

int main() {
    std::cout << "==========================================================\n";
    std::cout << "    CSR vs Bitmask  \n";
    std::cout << "==========================================================\n\n";

    struct TestConfig {
        std::string name;
        math::Graph* graph;
    };

    std::vector<TestConfig> tests;

    // Графы для тестирования (размер не должен превышать 64 узла)
    tests.push_back({ "grid 3x3", math::GraphGenerator::generateGrid(2, 2) });
    tests.push_back({ "grid 4x4", math::GraphGenerator::generateGrid(3, 3) });
    tests.push_back({ "grid 5x5", math::GraphGenerator::generateGrid(4, 4) });
    tests.push_back({ "grid 5x6", math::GraphGenerator::generateGrid(4, 5) });
    tests.push_back({ "grid 6x6", math::GraphGenerator::generateGrid(5, 5) });

    for (auto& test : tests) {
        std::cout << "--- Test: " << test.name << " (" << test.graph->getVertNumb() << " nodes) ---\n";

        // --- 1. (Bitmask) ---
        GraphMask mask = convertToMask(test.graph);
        auto start_mask = std::chrono::high_resolution_clock::now();
        double rel_mask = factoring_mask(mask);
        auto end_mask = std::chrono::high_resolution_clock::now();
        double time_mask = std::chrono::duration<double>(end_mask - start_mask).count();

        // --- 2. CSR (KAO-FO) ---
        auto start_csr = std::chrono::high_resolution_clock::now();
        // Используем последовательное исполнение для честного сравнения архитектур данных
        double rel_csr = math::GraphFactory::factoring(test.graph, math::FactoringType::FACTORING, math::FactoringExecutionType::SERIAL);
        auto end_csr = std::chrono::high_resolution_clock::now();
        double time_csr = std::chrono::duration<double>(end_csr - start_csr).count();

        std::cout << std::fixed << std::setprecision(6);
        std::cout << "CSR (KAO-FO)  | Time: " << time_csr << " s | Reliability: " << rel_csr << "\n";
        std::cout << "Bitmask       | Time: " << time_mask << " s | Reliability: " << rel_mask << "\n";

        if (time_mask > 0) {
            std::cout << "speedup (Bitmask faster CSR in): " << time_csr / time_mask << " times\n";
        }
        std::cout << "\n";

        delete test.graph;
    }

   
    return 0;
}