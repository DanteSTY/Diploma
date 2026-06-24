//раскомент если не нужно сравнение в benchmark comparision
/*

#include "GraphUtil.h"
#include "Graph.h"
#include <iomanip>
#include <iostream>
#include <chrono>
#include <omp.h>
#include <string>
#include <vector>

struct TestResults {
    double baseTaskWait = 0.0;
    double baseNoTaskWait = 0.0;
};

int main() {
    
    omp_set_dynamic(0);

    std::cout << std::fixed << std::setprecision(4);
    std::vector<int> test_threads = { 1, 2, 4, 6, 8, 12 };

    TestResults stats[3];

    //  (Warm-up) 
 
    std::cout << "Warming up OpenMP thread pool...\n";
 #pragma omp parallel
    {
        int tid = omp_get_thread_num(); 
    }

    for (int threads : test_threads) {
        omp_set_num_threads(threads);
        std::cout << "\n==========================================================\n";
        std::cout << ">>> RUNNING WITH THREADS: " << threads << " <<<\n";
        std::cout << "==========================================================\n";

        
        auto genK13 = []() { return math::GraphGenerator::generateCompleteGraph(13); };
        auto genGrid = []() { return math::GraphGenerator::generateGrid(6, 6); }; 
        auto genGeant = []() { return math::GraphGenerator::generateGEANT(); }; 

        auto run = [&](int idx, std::string name, auto genFunc, TestResults& st) {
            std::cout << "\n--- TEST: " << name << " ---\n";

            // taskwait
            math::Graph* g1 = genFunc();
            auto b1 = std::chrono::steady_clock::now();
            double rel1 = math::GraphFactory::factoring(g1, math::FactoringType::FACTORING, math::FactoringExecutionType::PARALLEL);
            auto e1 = std::chrono::steady_clock::now();

            
            std::chrono::duration<double> elapsed1 = e1 - b1;
            double t1 = elapsed1.count();
            if (threads == 1) st.baseTaskWait = t1;
            delete g1;

            // without taskwait
            math::Graph* g2 = genFunc();
            auto b2 = std::chrono::steady_clock::now();
            double rel2 = math::GraphFactory::factoring(g2, math::FactoringType::NO_REVERSE_REC_FACTORING, math::FactoringExecutionType::PARALLEL);
            auto e2 = std::chrono::steady_clock::now();

            std::chrono::duration<double> elapsed2 = e2 - b2;
            double t2 = elapsed2.count();
            if (threads == 1) st.baseNoTaskWait = t2;
            delete g2;

            double sp1 = (st.baseTaskWait > 0) ? (st.baseTaskWait / t1) : 1.0;
            double sp2 = (st.baseNoTaskWait > 0) ? (st.baseNoTaskWait / t2) : 1.0;

            
            std::cout << "  [WITH taskwait] Time: " << std::setw(8) << (t1 * 1000.0) << " ms | Speedup: " << sp1 << "x | Rel: " << rel1 << "\n";
            std::cout << "  [NO taskwait]   Time: " << std::setw(8) << (t2 * 1000.0) << " ms | Speedup: " << sp2 << "x | Rel: " << rel2 << "\n";
            std::cout << "  => ADVANTAGE: " << (t1 / t2) << "x\n";
            };

        //run(0, "Complete K13", genK13, stats[0]);
        //run(1, "Grid 6x6", genGrid, stats[1]);
        run(0, "Geant", genGeant, stats[2]);
    }

    std::cin.get();
    return 0;
}



/*

   std::ifstream input("src/grid55.txt");       

   if (!input.is_open()) {
       std::cout << "DIDNT OPEN! \n";
       return 1;
   }

   math::Graph graph;
   input >> graph;
  
  */
 


/*

void print(math::Graph* graph) {

    u_int* kao = graph->getKAO();
    u_int* fo = graph->getFO();

    std::cout << "graph vert num: " << graph->getVertNumb() << std::endl;
    std::cout << "graph edge num: " << graph->getEdgNumb() << std::endl;

    for (int i = 0; i < graph->getVertNumb(); i++) {
        std::cout << kao[i] << " ";
    }

    std::cout << std::endl;

    for (int i = 0; i < 2*graph->getEdgNumb(); i++) {
        std::cout << fo[i] << " ";
    }
}

*/