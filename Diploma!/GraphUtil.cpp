#include "GraphUtil.h"

#include "Graph.h"
#include "GeantGenerator.h"

#include <iostream>
#include <queue>
#include <stack>
#include <omp.h>
namespace math {
    
    int GraphFactory::count_of_triangles_arr[64] = { 0 };
    int GraphFactory::count_of_edges_degs_6_arr[64] = { 0 };
    int GraphFactory::count_first_triangles_arr[64] = { 0 };
    int GraphFactory::recursive_deep_arr[64] = { 0 };
    
    GraphFactory::AlignedDouble GraphFactory::rel_arr[64] = { 0.0 };


    Graph *GraphGenerator::generateCompleteGraph(u_int n) {
    u_int gVertNumb = n;
    u_int gEdgNumb = n * (n - 1) / 2;
    u_int *gKAO = new u_int[gVertNumb + 1];
    u_int *gFO = new u_int[2 * gEdgNumb];
    double *gFORel = new double[2 * gEdgNumb];

    u_int iC = 0;
    gKAO[0] = 0;
    for (u_int i = 1; i <= gVertNumb; i++) {
        gKAO[i] = gKAO[i - 1] + (n - 1);
    }
    for (u_int i = 1; i <= gVertNumb; i++) {
        for (u_int j = 1; j <= gVertNumb; j++) {
            if (i == j) {
                continue;
            }
            gFO[iC] = j;
            gFORel[iC] = 0.75;//        probability of edge          
            iC++;
        }
    }

    Graph *result = new Graph(gVertNumb, gEdgNumb, gKAO, gFO, gFORel);
    return result;
}

Graph *GraphGenerator::generateGrid(u_int n, u_int m) {
    if (m == 0) {
        m = n;
    }


    u_int gVertNumb = (n + 1) * (m + 1);
    u_int gEdgNumb = n * (m + 1) + m * (n + 1);
    u_int *gKAO = new u_int[gVertNumb + 1];
    u_int *gFO = new u_int[2 * gEdgNumb];
    double *gFORel = new double[2 * gEdgNumb];

    for (u_int i = 0; i < (2 * gEdgNumb); i++) {
        gFORel[i] = 0.75;
    }

    u_int iC = 1;
    u_int jC = -1;
    gKAO[0] = 0;
    for (u_int k = 0; k < (m + 1); k++) {       
        if (k == 0) {
            for (u_int i = 0; i < (n + 1); i++) {
                if (i == 0) {                   
                    gKAO[iC] = gKAO[iC - 1] + 2;
                    gFO[++jC] = iC + 1;
                    gFO[++jC] = iC  + (n + 1);                    
                }
                else if (i == n) {              
                    gKAO[iC] = gKAO[iC - 1] + 2;
                    gFO[++jC] = iC - 1;
                    gFO[++jC] = iC + (n + 1);
                }
                else {
                    gKAO[iC] = gKAO[iC - 1] + 3;
                    gFO[++jC] = iC - 1;
                    gFO[++jC] = iC + 1;
                    gFO[++jC] = iC + (n + 1);
                }
                iC++;
            }
        }
        else if (k == m) {
            for (u_int i = 0; i < (n + 1); i++) {
                if (i == 0) {                   
                    gKAO[iC] = gKAO[iC - 1] + 2;
                    gFO[++jC] = iC - (n + 1);
                    gFO[++jC] = iC + 1; 
                }
                else if (i == n) {              
                    gKAO[iC] = gKAO[iC - 1] + 2;
                    gFO[++jC] = iC - (n + 1);
                    gFO[++jC] = iC - 1;
                }
                else {
                    gKAO[iC] = gKAO[iC - 1] + 3;
                    gFO[++jC] = iC - (n + 1);
                    gFO[++jC] = iC - 1;
                    gFO[++jC] = iC + 1;
                }
                iC++;
            }
        }
        else {
            for (u_int i = 0; i < (n + 1); i++) {
                if (i == 0) {
                    gKAO[iC] = gKAO[iC - 1] + 3;
                    gFO[++jC] = iC - (n + 1);
                    gFO[++jC] = iC + 1;
                    gFO[++jC] = iC + (n + 1);
                }
                else if (i == n) {
                    gKAO[iC] = gKAO[iC - 1] + 3;
                    gFO[++jC] = iC - (n + 1);
                    gFO[++jC] = iC - 1;
                    gFO[++jC] = iC + (n + 1);
                }
                else {
                    gKAO[iC] = gKAO[iC - 1] + 4;
                    gFO[++jC] = iC - (n + 1);
                    gFO[++jC] = iC - 1;
                    gFO[++jC] = iC + 1;
                    gFO[++jC] = iC + (n + 1);
                }
                iC++;
            }
        }
    }

    Graph *result = new Graph(gVertNumb, gEdgNumb, gKAO, gFO, gFORel);
    return result;
}

Graph *GraphGenerator::generateGridWithDiags(u_int n, u_int m) {
    if (m == 0) {
        m = n;
    }
    
    u_int gVertNumb = (n + 1) * (m + 1);
    u_int gEdgNumb = 4 * n * m + n + m;
    u_int *gKAO = new u_int[gVertNumb + 1];
    u_int *gFO = new u_int[2 * gEdgNumb];
    double *gFORel = new double[2 * gEdgNumb];

    for (u_int i = 0; i < (2 * gEdgNumb); i++) {
        gFORel[i] = 0.75;
    }

    u_int iC = 1;
    u_int jC = -1;
    gKAO[0] = 0;
    for (u_int k = 0; k < (m + 1); k++) {
        if (k == 0) {
            for (u_int i = 0; i < (n + 1); i++) {
                if (i == 0) {                   
                    gKAO[iC] = gKAO[iC - 1] + 3;
                    gFO[++jC] = iC + 1;
                    gFO[++jC] = iC + (n + 1);
                    gFO[++jC] = iC + (n + 2);
                }
                else if (i == n) {              
                    gKAO[iC] = gKAO[iC - 1] + 3;
                    gFO[++jC] = iC - 1;
                    gFO[++jC] = iC + n;
                    gFO[++jC] = iC + (n + 1);
                }
                else {
                    gKAO[iC] = gKAO[iC - 1] + 5;
                    gFO[++jC] = iC - 1;
                    gFO[++jC] = iC + 1;
                    gFO[++jC] = iC + n;
                    gFO[++jC] = iC + (n + 1);
                    gFO[++jC] = iC + (n + 2);
                }
                iC++;
            }
        }
        else if (k == m) {
            for (u_int i = 0; i < (n + 1); i++) {
                if (i == 0) {                   
                    gKAO[iC] = gKAO[iC - 1] + 3;
                    gFO[++jC] = iC - (n + 1);
                    gFO[++jC] = iC - n;
                    gFO[++jC] = iC + 1;
                }
                else if (i == n) {              
                    gKAO[iC] = gKAO[iC - 1] + 3;
                    gFO[++jC] = iC - (n + 2);
                    gFO[++jC] = iC - (n + 1);
                    gFO[++jC] = iC - 1;
                }
                else {
                    gKAO[iC] = gKAO[iC - 1] + 5;
                    gFO[++jC] = iC - (n + 2);
                    gFO[++jC] = iC - (n + 1);
                    gFO[++jC] = iC - n;
                    gFO[++jC] = iC - 1;
                    gFO[++jC] = iC + 1;
                }
                iC++;
            }
        }
        else {
            for (u_int i = 0; i < (n + 1); i++) {
                if (i == 0) {
                    gKAO[iC] = gKAO[iC - 1] + 5;
                    gFO[++jC] = iC - (n + 1);
                    gFO[++jC] = iC - n;
                    gFO[++jC] = iC + 1;
                    gFO[++jC] = iC + (n + 1);
                    gFO[++jC] = iC + (n + 2); 
                }
                else if (i == n) {
                    gKAO[iC] = gKAO[iC - 1] + 5;
                    gFO[++jC] = iC - (n + 2);
                    gFO[++jC] = iC - (n + 1);
                    gFO[++jC] = iC - 1;
                    gFO[++jC] = iC + n;
                    gFO[++jC] = iC + (n + 1); 
                }
                else {
                    gKAO[iC] = gKAO[iC - 1] + 8;
                    gFO[++jC] = iC - (n + 2);
                    gFO[++jC] = iC - (n + 1);
                    gFO[++jC] = iC - n;
                    gFO[++jC] = iC - 1;
                    gFO[++jC] = iC + 1;
                    gFO[++jC] = iC + n;
                    gFO[++jC] = iC + (n + 1);
                    gFO[++jC] = iC + (n + 2);
                }
                iC++;
            }
        }
    }

    Graph *result = new Graph(gVertNumb, gEdgNumb, gKAO, gFO, gFORel);
    return result;
}

Graph *GraphGenerator::generateGraph(u_int vNumb, u_int eNumb) {
    return nullptr;     //not supported yet
}

Graph *GraphGenerator::generateGEANT() {
    u_int gVertNumb = 103;
    u_int gEdgNumb = 127;
    u_int *gKAO = new u_int[gVertNumb + 1];
    u_int *gFO = new u_int[2 * gEdgNumb];
    double *gFORel = new double[2 * gEdgNumb];

    for (u_int i = 0; i < (2 * gEdgNumb); i++) {
        gFORel[i] = 0.75;
    }

    GEANTGenerator::GEANTGenerate(gKAO, gFO);

    Graph *result = new Graph(gVertNumb, gEdgNumb, gKAO, gFO, gFORel);
    return result;
}




//int GraphFactory::count_of_triangles = 0;
//int GraphFactory::count_of_edges_degs_6 = 0;
//int GraphFactory::count_first_triangles = 0;


/*
                
*/

//We take an edge with a degree sum less than 7 and select it without looking. If no such edges are found (we're inside the dense part of the graph), the algorithm doesn't give up.
// It remembers the edge with the smallest sum of all the edges it saw (the variable deg).
/*

void GraphFactory::choseVerts(Graph*& graph, double& p, u_int& u, u_int& v, u_int& uC, u_int& vC) {
    vC = graph->vertNumb;
    u_int deg = 2 * graph->vertNumb + 2;
    int tid = omp_get_thread_num(); //          ID                      

    for (u_int i = (2 * graph->edgNumb - 1); i >= graph->KAO[1]; i--) {
        uC = graph->FO[i];
        if (i < graph->KAO[vC - 1]) vC--;

        if (vC > uC) {
            u_int current_deg_sum = graph->KAO[uC] - graph->KAO[uC - 1] + graph->KAO[vC] - graph->KAO[vC - 1];

            if (current_deg_sum < 7) {
                u = uC; v = vC; p = graph->FORel[i];

                //                     
                if (current_deg_sum == 6) {
                    GraphFactory::count_of_edges_degs_6_arr[tid]++;
                }
                //           :                                , 
                //         count_of_triangles           0
                return;
            }
            else if (current_deg_sum < deg) {
                u = uC; v = vC; p = graph->FORel[i];
                deg = current_deg_sum;
            }
        }
    }
}

*/

/* 
                              
*/


/*
void GraphFactory::choseVerts(Graph *&graph, double &p, u_int &u, u_int &v, u_int &uC, u_int &vC) {
     
    u = graph->vertNumb;
    int indexU = graph->KAO[u-1];
    int degU = graph->KAO[u] - graph->KAO[u - 1];
    v = graph->FO[indexU + degU - 1];
    uC = u;
    vC = v;

    p = graph->FORel[2 * graph->edgNumb - 1];
}
*/



//                     


/*
#include <cstdlib>
#include <ctime>

void GraphFactory::choseVerts(Graph*& graph, double& p, u_int& u, u_int& v, u_int& uC, u_int& vC) {

    std::srand(std::time(0));
    
    u = rand() % (graph->getVertNumb()-1) + 1;

    int deg = graph->KAO[u] - graph->KAO[u-1];

    int k = (rand() % deg);

    v = graph->FO[graph->KAO[u-1] + k];
    p = graph->FORel[u];

    uC = u;
    vC = v;

}
*/


/*
                                                                     
*/
    
/*
void GraphFactory::choseVerts(Graph*& graph, double& p, u_int& u, u_int& v, u_int& uC, u_int& vC) {

    int degU = graph->KAO[graph->vertNumb] - graph->KAO[graph->vertNumb-1];
    int min_deg = 100000;
    int degV;

    u = graph->vertNumb;
    uC = u;

    for (size_t i = 2 * graph->edgNumb - 1; i > 2 * graph->edgNumb - 1 - degU; i--) {
        degV = graph->KAO[graph->FO[i]] - graph->KAO[graph->FO[i-1]];
        
        if(degU + degV < min_deg && graph->isEdge(u, i)) 
        {
            v = graph->FO[i];
            p = graph->FORel[u];
            vC = v;
        }
    }

}
*/



/*
     (Priority 1 (Ideal Option): Look for an edge with a degree sum of 6 that is also part of a triangle.
Priority 2 (Fallback "Thin" Option): If there are no triangles with a degree sum of 6, take the first edge with a degree sum of 6 (looking from the end of the array).
Priority 3 (Extreme Case): If there are no edges with a degree sum of 6 in the graph, take the edge with the minimum possible degree sum (e.g., 7 or 8).
*/

 
void GraphFactory::choseVerts(Graph*& graph, double& p, u_int& u, u_int& v, u_int& uC, u_int& vC) {
    int min_degSum = 10000000;
    int tmp_u, tmp_v;
    double tmp_p; //                                     

    int flag = 0;
    int cft = 0;

    for (size_t i = graph->vertNumb; i > 0; i--) {
        int indexU = graph->KAO[i - 1] - 1;
        int degU = graph->KAO[i] - graph->KAO[i - 1];

        for (size_t j = degU; j > 0; j--) {
            int V = graph->FO[indexU + j];
            int degV = graph->KAO[V] - graph->KAO[V - 1];

            if (degU + degV == 6) {
                GraphFactory::count_of_edges_degs_6_arr[omp_get_thread_num()]++;

                for (size_t k = degU; k > 0; k--) {
                    int UV = graph->FO[indexU + k];
                    int indexUV = graph->KAO[UV - 1];

                    if (UV != V) {
                        int degUV = graph->KAO[UV] - graph->KAO[UV - 1];

                        for (size_t l = 0; l < degUV; l++) {
                            if (graph->FO[indexUV + l] == V) {
                                GraphFactory::count_of_triangles_arr[omp_get_thread_num()]++;
                                u = i;
                                v = V;
                                uC = u;
                                vC = v;
                                
                                p = graph->FORel[indexU + j];

                                if (cft == 0)
                                    GraphFactory::count_first_triangles_arr[omp_get_thread_num()]++;
                                return;
                            }
                        }
                    }
                }//for k

                cft++;

                if (flag == 0) {
                    tmp_u = i;
                    tmp_v = V;
                    //                       
                    tmp_p = graph->FORel[indexU + j];
                    flag++;
                }

            }//if deg1 + deg2 == 6
            else if (min_degSum > degU + degV) {
                min_degSum = degU + degV;
                u = i;
                v = V;
                uC = u;
                vC = v;
                
                p = graph->FORel[indexU + j];
            }
        }//for j
    }//for i 

    if (flag != 0) {
        u = tmp_u;
        v = tmp_v;
        uC = u;
        vC = v;
        //                              
        p = tmp_p;
    }
}

















//____________________________________________________FACTORING____________________________________________________



void GraphFactory::branchingWithNoReverse(Graph *&graph, double &rel, double prob) {
    //recursive_deep++;
    //recursive_deep.fetch_add(1);
    GraphFactory::recursive_deep_arr[omp_get_thread_num()]++;

    u_int u, v, uC, vC;
    double pstFactor, p;

    graph->parallelSeriesTransformation(pstFactor);
    //if (graph->getVertNumb() > 5)
    if (graph->getVertNumb() > 3) {
        choseVerts(graph, p, u, v, uC, vC);
        Graph *merge = graph->mergeEdge(u, v);
        Graph *cut = graph->cutEdge(u, v);
        if (cut->isConnective()) {
            branchingWithNoReverse(cut, rel, pstFactor * (1 - p) * prob);
        }
        branchingWithNoReverse(merge, rel, pstFactor * p * prob);
        delete merge;
        delete cut;
    }
    else {     
        rel += pstFactor * prob * graph->baseProbabilities();
        return;
    }
}


//параллелизация без обратного хода
void GraphFactory::branchingWithNoReverse_parallel(Graph* graph, double prob) {
    int tid = omp_get_thread_num();
    GraphFactory::recursive_deep_arr[tid]++;

    u_int u, v, uC, vC;
    double pstFactor, p;

    // do series-parallel reduction
    graph->parallelSeriesTransformation(pstFactor);

    // (Cutoff)
    // If the graph is small, we count it sequentially in the current flow
    if (graph->getVertNumb() <= 10) {
        double local_rel = 0.0;
        //  (prob * pstFactor)
        branchingWithNoReverse(graph, local_rel, prob * pstFactor);

        
        rel_arr[tid].val += local_rel;
        return;
    }

    // choose edge
    choseVerts(graph, p, u, v, uC, vC);

    
    Graph* merge = graph->mergeEdge(u, v);
    Graph* cut = graph->cutEdge(u, v);
    bool is_connective = cut->isConnective();

    //(Merge)
#pragma omp task firstprivate(merge, prob, pstFactor, p)
    {
        branchingWithNoReverse_parallel(merge, prob * pstFactor * p);
        delete merge; 
    }

    //  (Cut)
    if (is_connective) {
#pragma omp task firstprivate(cut, prob, pstFactor, p)
        {
            branchingWithNoReverse_parallel(cut, prob * pstFactor * (1.0 - p));
            delete cut; 
        }
    }
    else {
        
        delete cut;
    }

    
}






//ветвление
double GraphFactory::branching(Graph*& graph) {
    GraphFactory::recursive_deep_arr[omp_get_thread_num()]++;
    u_int u, v, uC, vC;
    double pstFactor, p, result;

    graph->parallelSeriesTransformation(pstFactor);

    // if (graph->getVertNumb() > 5)
    if (graph->getVertNumb() > 3) {
        choseVerts(graph, p, u, v, uC, vC);
        Graph* merge = graph->mergeEdge(u, v);
        Graph* cut = graph->cutEdge(u, v);

        if (cut->isConnective()) {
            result = pstFactor * (p * branching(merge) + (1 - p) * branching(cut));
        }
        else {
            result = pstFactor * (p * branching(merge));
        }
        delete merge;
        delete cut;
        return result;
    }
    else {
        // Сюда алгоритм зайдет только для 1, 2 или 3 узлов.
        // Формулы для <= 3 работают идеально математически точно!
        return pstFactor * graph->baseProbabilities();
    }
}
//паралллизация(ветвление) с обратным ходом
double GraphFactory::branching_parallel(Graph* graph) {
    GraphFactory::recursive_deep_arr[omp_get_thread_num()]++;

    u_int u, v, uC, vC;
    double pstFactor, p, result = 0;

    graph->parallelSeriesTransformation(pstFactor);

    // cutoff threshold      
    if (graph->getVertNumb() <= 10) {
        return pstFactor * branching(graph);
    }

    choseVerts(graph, p, u, v, uC, vC);

    Graph* merge = graph->mergeEdge(u, v);
    Graph* cut = graph->cutEdge(u, v);

    double res_merge = 0, res_cut = 0;
    bool is_connective = cut->isConnective();

   
#pragma omp task shared(res_merge) firstprivate(merge)
    {
        res_merge = branching_parallel(merge);
        delete merge;
    }

    if (is_connective) {
#pragma omp task shared(res_cut) firstprivate(cut)
        {
            res_cut = branching_parallel(cut);
            delete cut;
        }
    }
    else {
        delete cut; 
    }

   
                            
#pragma omp taskwait 

    
    return pstFactor * (p * res_merge + (1.0 - p) * res_cut);
}


//факторизация
double GraphFactory::factoring(Graph*& graph, FactoringType factoringType, FactoringExecutionType execType) {
  
    for (int i = 0; i < 64; i++) {
        recursive_deep_arr[i] = 0;
        count_of_triangles_arr[i] = 0;
        count_of_edges_degs_6_arr[i] = 0;
        count_first_triangles_arr[i] = 0;
        rel_arr[i].val = 0.0;
    }

    //  preparotary reduction
    double shootsProbability = 1.0;
    //graph->shootsDeletion(shootsProbability); 

    if (shootsProbability == 0.0) return 0.0;

    double final_res = 0.0;

    if (execType == PARALLEL) {
#pragma omp parallel
        {
#pragma omp single
            {
                if (factoringType == FACTORING) {
                    final_res = branching_parallel(graph) * shootsProbability;
                }
                else if (factoringType == NO_REVERSE_REC_FACTORING) {
                    branchingWithNoReverse_parallel(graph, shootsProbability);
                }
            }
            
        }

        
        if (factoringType == NO_REVERSE_REC_FACTORING) {
            int num_threads = omp_get_max_threads();
            for (int i = 0; i < num_threads; i++) {
                final_res += rel_arr[i].val;
            }
        }
        return final_res;
    }
    else {
        
        if (factoringType == FACTORING) {
            return branching(graph) * shootsProbability;
        }
        else {
            double res_no_rev = 0.0;
            branchingWithNoReverse(graph, res_no_rev, shootsProbability);
            return res_no_rev;
        }
    }
}

















}