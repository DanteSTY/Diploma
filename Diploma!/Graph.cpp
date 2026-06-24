#include "Graph.h"

#include "list.h"

using namespace containers;

namespace math {

void Graph::memclear() {
    delete [] KAO;
    delete [] FO;
    delete [] FORel;

    KAO = nullptr;
    FO = nullptr;
    FORel = nullptr;
    vertNumb = 0;
    edgNumb = 0;
}

void Graph::memcopy(const Graph &graph) {
    vertNumb = graph.vertNumb;
    edgNumb = graph.edgNumb;

    KAO = new u_int[vertNumb + 1];
    FO = new u_int[2 * edgNumb];
    FORel = new double[2 * edgNumb];


    for (u_int i = 0; i <= vertNumb; i++) {
        KAO[i] = graph.KAO[i];
    }
    for (u_int i = 0; i < (2 * edgNumb); i++) {
        FO[i] = graph.FO[i];
        FORel[i] = graph.FORel[i];
    }
}

#if __cplusplus >= 201103L
void Graph::memmove(Graph &&other) {
    KAO = other.KAO;
    FO = other.FO;
    FORel = other.FORel;

    vertNumb = other.vertNumb;
    edgNumb = other.edgNumb;

    other.KAO = nullptr;
    other.FO = nullptr;
    other.FORel = nullptr;
    other.vertNumb = 0;
    other.edgNumb = 0;
}
#endif

void Graph::memallocNewGraph(u_int _vertNumb, u_int _edgNumb, u_int *&newKAO, u_int *&newFO, double *&newFORel) const {
    newKAO = new u_int[_vertNumb + 1];
    newFO = new u_int[2 * _edgNumb];
    newFORel = new double[2 * _edgNumb];
}



void Graph::memupdate(u_int vertLim, u_int edgLim, u_int *&_KAO, u_int *&_FO, double *&_FORel) {
    if (vertLim > vertNumb) {
        return;             //raise exception here
    }
    if (edgLim > edgNumb) {
        return;             //raise exception here
    }
    
    for (u_int i = 0; i <= vertLim; i++) {
        KAO[i] = _KAO[i];
    }

    for (u_int i = 0; i < (2*edgLim); i++) {
        FO[i] = _FO[i];
        FORel[i] = _FORel[i];
    }
}

void Graph::memrebase(u_int vertLim, u_int edgLim, u_int *&_KAO, u_int *&_FO, double *&_FORel) {
    memclear();
    vertNumb = vertLim;
    edgNumb = edgLim;
    KAO = _KAO;
    FO = _FO;
    FORel = _FORel;
    //memallocNewGraph(vertLim, edgLim, KAO, FO, FORel);
    //memupdate(vertLim, edgLim, _KAO, _FO, _FORel);
}

bool Graph::meminited() const {
    return (vertNumb != 0) && (edgNumb != 0);
}

bool Graph::isEdge(u_int u, u_int v) const {
    if (u == v) {
        return false;
    }

    for (u_int i = KAO[u - 1]; i < KAO[u]; i++) {
        if (FO[i] == v) {
            return true;
        }
    }
    return false;
}

bool Graph::isEdgeWithPos(u_int u, u_int v, u_int &pos) const {
    if (u == v) {
        return false;
    }

    for (u_int i = KAO[u - 1]; i < KAO[u]; i++) {
        if (FO[i] == v) {
            pos = i;
            return true;
        }
    }
    return false;
}

#if __cplusplus >= 201103L
Graph::Graph(): KAO(nullptr), FO(nullptr), FORel(nullptr), vertNumb(0), edgNumb(0) {}

Graph::Graph(Graph &&other) {
    memmove(std::forward<Graph>(other));
}
#else
    Graph::Graph(): KAO(NULL), FO(NULL), FORel(NULL), vertNumb(0), edgNumb(0) {}
#endif

Graph::Graph(const Graph &graph) {
    memcopy(graph);
}

Graph::Graph(u_int _vertNumb, u_int _edgNumb, u_int *&_KAO, u_int *&_FO, double *&_FORel): vertNumb(_vertNumb), edgNumb(_edgNumb) {
    KAO = _KAO;
    FO = _FO;
    FORel = _FORel;

}

Graph::~Graph() {
    memclear();
} 

std::ifstream &operator >>(std::ifstream &input, Graph &graph) {
    input >> graph.vertNumb;
    input >> graph.edgNumb;


    graph.KAO = new u_int[graph.vertNumb + 1];
    graph.FO = new u_int[2 * graph.edgNumb];
    graph.FORel = new double[2 * graph.edgNumb];

    for (u_int i = 0; i <= graph.vertNumb; i++) {
        input >> graph.KAO[i];
    }

    for (u_int i = 0; i < (graph.edgNumb * 2); i++) {
        input >> graph.FO[i];
        graph.FORel[i] = 0.75;
    }


    return input;
}

Graph &Graph::operator= (const Graph &graph) {
    if (this == &graph) {
        return *this;
    }

    if (meminited()) {
        memclear();
    }
    memcopy(graph);

    return *this;
}

Graph &Graph::operator=(const Graph *graph) {
    if (this == graph) {
        return *this;
    }

    if (meminited()) {
        memclear();
    }
    memcopy(*graph);

    return *this;
}

#if __cplusplus >= 201103L
Graph &Graph::operator= (Graph &&other) {
    if (meminited()) {
        memclear();
    }

    memmove(std::forward<Graph>(other));

    return *this;
}
#endif

Graph *Graph::mergeEdge(u_int u, u_int v) const {
    if ((u == 0) || (u > vertNumb) || (v == 0) || (v > vertNumb) || (!isEdge(u, v))) {
        return nullptr;     //raise exception here
    }
    u_int newVertNumb = vertNumb - 1;
    u_int newEdgNumb = edgNumb - 1;
    u_int *newKAO;
    u_int *newFO;
    double *newFORel;
    u_int newVert;      //номер вершины, в которую осуществл€етс€ ст€гивание (выбираетс€ наименьша€ по номеру из пары (u, v))
    u_int delVert;      //номер вершины, котора€ будет удалена из графа

    if (u < v) {
        newVert = u;
        delVert = v;
    }
    else if (v < u) {
        newVert = v;
        delVert = u;
    }
    else {
        return nullptr;         //raise exception here
    }

    for (u_int i = KAO[newVert - 1]; i < KAO[newVert]; i++) {
        if (FO[i] == v) {
            continue;
        }
        if (isEdge(FO[i], delVert)) {
            newEdgNumb--;
        }
    }

    memallocNewGraph(newVertNumb, newEdgNumb, newKAO, newFO, newFORel);

    newKAO[0] = 0;
    u_int iC = 1;
    u_int jC = 0;
    u_int pos = 0;
    for (u_int i = 1; i <= vertNumb; i++) {
        if (i == delVert) {
            continue;
        }
        else if (i == newVert) {
            newKAO[iC] = newKAO[iC - 1] + (KAO[i] - KAO[i - 1]) + (KAO[delVert] - KAO[delVert - 1]) - 2;
            for (u_int j = KAO[delVert - 1]; j < KAO[delVert]; j++) {
                if (FO[j] == newVert) {
                    continue;
                }
                if (!isEdge(FO[j], newVert)) {
                    newFO[jC] = FO[j];
                    newFORel[jC] = FORel[j];
                    if (FO[j] > delVert) {
                        newFO[jC]--;
                    }
                    jC++;
                }
                else {
                    continue;
                }
            }
            for (u_int j = KAO[i - 1]; j < KAO[i]; j++) {
                if (FO[j] == delVert) {
                    continue;
                }
                if (isEdgeWithPos(FO[j], delVert, pos)) {
                    newKAO[iC]--;
                    newFORel[jC] = FORel[j] + FORel[pos] - FORel[j]*FORel[pos];
                }
                else {
                    newFORel[jC] = FORel[j];
                }
                newFO[jC] = FO[j];
                if (FO[j] > delVert) {
                    newFO[jC]--;
                }
                jC++; 
            }
        }
        else {
            u_int posNew = 0;
            u_int posDel = 0;
            newKAO[iC] = newKAO[iC - 1] + (KAO[i] - KAO[i - 1]);
            bool newVertConnected = isEdgeWithPos(i, newVert, posNew);
            bool delVertConnected = isEdgeWithPos(i, delVert, posDel);

            if (!delVertConnected) {
                for (u_int j = KAO[i - 1]; j < KAO[i]; j++) {
                    newFO[jC] = FO[j];
                    newFORel[jC] = FORel[j];
                    if (FO[j] > delVert) {
                        newFO[jC]--;
                    }
                    jC++;
                }
            }
            else if (!newVertConnected && delVertConnected) {
                for (u_int j = KAO[i - 1]; j < KAO[i]; j++) {
                    if (FO[j] == delVert) {
                        newFO[jC] = newVert;
                    }
                    else {
                        newFO[jC] = FO[j];
                        if (FO[j] > delVert) {
                            newFO[jC]--;
                        }
                    }
                    newFORel[jC] = FORel[j];
                    jC++;
                }
            }
            else if (newVertConnected && delVertConnected) {
                newKAO[iC]--;
                for (u_int j = KAO[i - 1]; j < KAO[i]; j++) {
                    if (FO[j] == delVert) {
                        continue;
                    }
                    newFO[jC] = FO[j];
                    if (FO[j] == newVert) {
                        newFORel[jC] = FORel[j] + FORel[posDel] - FORel[j]*FORel[posDel];
                    }
                    else {
                        newFORel[jC] = FORel[j];
                    }
                    if (FO[j] > delVert) {
                            newFO[jC]--;
                        }
                    jC++;
                }
            }
        }

        iC++;
    }
    
    Graph *result = new Graph(newVertNumb, newEdgNumb, newKAO, newFO, newFORel);
    return result;
}

Graph *Graph::cutEdge(u_int u, u_int v) const {
    // ЅџЋќ: if ((u == 0) || (u > edgNumb) || (v == 0) || (v > edgNumb) || (!isEdge(u, v))) и в mergeEdge, deg
    if ((u == 0) || (u > vertNumb) || (v == 0) || (v > vertNumb) || (!isEdge(u, v))) {
        return nullptr;     //raise exception here
    }

    u_int newVertNumb = vertNumb;
    u_int newEdgNumb = edgNumb - 1;
    u_int *newKAO;
    u_int *newFO;
    double *newFORel;

    memallocNewGraph(newVertNumb, newEdgNumb, newKAO, newFO, newFORel);

    newKAO[0] = 0;
    u_int iC = 1;
    u_int jC = 0;
    for (u_int i = 1; i <= vertNumb; i++) {
        newKAO[iC] = newKAO[iC - 1] + (KAO[i] - KAO[i - 1]);
        if ((i == u) || (i == v)) {
            for (u_int j = KAO[i - 1]; j < KAO[i]; j++) {
                if (((i == u) && (FO[j] == v)) || ((i == v) && (FO[j] == u))) {
                    newKAO[iC]--;
                    continue;
                }
                else {
                    newFO[jC] = FO[j];
                    newFORel[jC] = FORel[j];
                    jC++;
                }
            }
        }
        else {
            for (u_int j = KAO[i - 1]; j < KAO[i]; j++) {
                newFO[jC] = FO[j];
                newFORel[jC] = FORel[j];
                jC++;
            }
        }

        iC++;
    }

    Graph *result = new Graph(newVertNumb, newEdgNumb, newKAO, newFO, newFORel);
    return result;
}

void Graph::shootsDeletion(double &prob) {
    prob = 1.0;
    UniqueSortedList<u_int> delVerts;
    u_int delVertNum = 0;
    u_int *newKAO;
    u_int *newFO;
    double *newFORel;

    for (u_int i = 1; i <= vertNumb; i++) {
        if ((KAO[i] - KAO[i - 1]) == 1) {
            prob *= FORel[KAO[i - 1]];
            delVerts.insert(i);
            u_int v = FO[KAO[i - 1]];
            u_int prevV = i;
            while (true) {
                if ((KAO[v] - KAO[v - 1]) == 2) {
                    delVerts.insert(i);
                    for (u_int j = KAO[v - 1]; j < KAO[v]; j++) {
                        if (FO[j] == prevV) {
                            continue;
                        }
                        prevV = v;
                        v = FO[j];
                        prob *= FORel[j];
                        break;
                    }
                }
                else {
                    break;
                }
            }
        }
    }

    delVertNum = delVerts.len();
    if (delVertNum == 0) {
        return;
    }
    memallocNewGraph(vertNumb - delVertNum, edgNumb - delVertNum, newKAO, newFO, newFORel);

    u_int currentPos;
    newKAO[0] = 0;
    u_int iC = 1;
    u_int jC = 0;
    for (u_int i = 1; i <= vertNumb; i++) {
        if (delVerts.inList(i)) {
            continue;
        }

        newKAO[iC] = newKAO[iC - 1] + (KAO[i] - KAO[i - 1]);
        for (u_int j = KAO[i - 1]; j < KAO[i]; j++) {
            if (delVerts.inList(FO[j])) {
                newKAO[iC]--;
                continue;
            }
            currentPos = delVerts.positionBetween(FO[j]);
            if (currentPos == 0) {
                newFO[jC] = FO[j];
            }
            else {
                newFO[jC] = FO[j] - currentPos;
            }
            newFORel[jC] = FORel[j];
            jC++;
        }

        iC++;
    }
    
    memclear();
    vertNumb -= delVertNum;
    edgNumb -= delVertNum;
    KAO = newKAO;
    FO = newFO;
    FORel = newFORel;
}

void Graph::parallelSeriesTransformation(double &prob) {
    prob = 1;
    u_int u, v, currentDelVert;
    u_int *newKAO;
    u_int *newFO;
    double *newFORel;
    double newProb, p1, p2;

    bool found2Deg = false;
    bool duplicate = false;
    for (u_int i = 1; i <= vertNumb; i++) {
        if ((KAO[i] - KAO[i - 1]) == 2) {
                currentDelVert = i;
                u = FO[KAO[i - 1]];
                v = FO[KAO[i] - 1];
                p1 = FORel[KAO[i - 1]];
                p2 = FORel[KAO[i] - 1];
                prob *= (p1 + p2 - p1 * p2);
                newProb = p1 * p2 / (p1 + p2 - p1 * p2);
                found2Deg = true;
                for (u_int j = KAO[u - 1]; j < KAO[u]; j++) {
                    if (FO[j] == v) {
                        duplicate = true;
                        newProb = 1 - (1 - FORel[j]) * (1 - newProb);
                        break;
                    }
                }
                break;
        }
    }

    if (!found2Deg) {
        return;
    }

    newKAO = new u_int[vertNumb];
    if (duplicate) {
        newFO = new u_int[2 * (edgNumb - 2)];
        newFORel = new double[2 * (edgNumb - 2)];
    }
    else {
        newFO = new u_int[2 * (edgNumb - 1)];
        newFORel = new double[2 *(edgNumb - 1)];
    }

    bool iterate = true;
    while (iterate) {
        newKAO[0] = 0; 
        u_int iC = 1;
        u_int jC = 0;
        for (u_int i = 1; i <= vertNumb; i++) {
            if (i == currentDelVert) {
                continue;
            }
            else if ((i == u) || (i == v)) {
                newKAO[iC] = newKAO[iC - 1] + KAO[i] - KAO[i - 1];
                if (duplicate) {
                    newKAO[iC]--;
                }
                for (u_int j = KAO[i - 1]; j < KAO[i]; j++) {
                    if (FO[j] == currentDelVert) {
                        if (duplicate) {
                            continue;
                        }
                        else {
                            if (i == u) {
                                newFO[jC] = v;
                                newFORel[jC] = newProb;
                            }
                            else {
                                newFO[jC] = u;
                                newFORel[jC] = newProb;
                            }
                        }
                    }
                    else if (((i == u) && (FO[j] == v)) || ((i == v) && (FO[j] == u))) {
                        newFO[jC] = FO[j];
                        newFORel[jC] = newProb;
                    }
                    else {
                        newFO[jC] = FO[j];
                        newFORel[jC] = FORel[j];
                    }

                    if (newFO[jC] > currentDelVert) {
                        newFO[jC]--;
                    }
                    jC++;
                } 
            }
            else {
                newKAO[iC] = newKAO[iC - 1] + KAO[i] - KAO[i - 1];
                for (u_int j = KAO[i - 1]; j < KAO[i]; j++) {
                    newFO[jC] = FO[j];
                    newFORel[jC] = FORel[j];
                    if (FO[j] > currentDelVert) {
                        newFO[jC]--;
                    }
                    jC++;
                }
            }

            iC++;
        }

        vertNumb--;
        if (duplicate) {
            edgNumb -= 2;
            duplicate = false;
        }
        else {
            edgNumb--;
        }

        found2Deg = false;
        for (u_int i = 1; i <= vertNumb; i++) {
            if ((newKAO[i] - newKAO[i - 1]) == 2) {
                    currentDelVert = i;
                    u = newFO[newKAO[i - 1]];
                    v = newFO[newKAO[i] - 1];
                    p1 = newFORel[newKAO[i - 1]];
                    p2 = newFORel[newKAO[i] - 1];
                    prob *= (p1 + p2 - p1 * p2);
                    newProb = p1 * p2 / (p1 + p2 - p1 * p2);
                    found2Deg = true;
                    for (u_int j = newKAO[u - 1]; j < newKAO[u]; j++) {
                        if (newFO[j] == v) {
                            duplicate = true;
                            newProb = 1 - (1 - newFORel[j]) * (1 - newProb);
                            break;
                        }
                    }
                    break;
            }
        }

        if (!found2Deg) {
            memrebase(vertNumb, edgNumb, newKAO, newFO, newFORel);
            iterate = false;
        }
        else {
            memupdate(vertNumb, edgNumb, newKAO, newFO, newFORel);
        }   
    }
}

bool Graph::isConnective() const {
    if (vertNumb == 1) {
        return true;
    }

    u_int startEdge = 1;
    u_int visitedSum = 1;
    list<u_int> queue;
    queue.push_back(startEdge);
    bool *visited = new bool[vertNumb + 1];
    visited[startEdge] = true;
    for (u_int i = 2; i <= vertNumb; i++) {
        visited[i] = false;
    }

    while (!queue.empty()) {
        if (visitedSum == vertNumb) {
            delete [] visited;
            return true;
        }
        u_int currentEdge = queue.front();
        queue.pop_front();
        for (u_int i = KAO[currentEdge - 1]; i < KAO[currentEdge]; i++) {
            if (!visited[FO[i]]) {
                visited[FO[i]] = true;
                visitedSum++;
                queue.push_back(FO[i]);
            }
        }
    }

    delete [] visited;
    return (visitedSum == vertNumb);      
}

double Graph::baseProbabilities() const {
    u_int SE;
    double result = 0;
    if (vertNumb == 1) {
        result = 1.0;
    }
    else if (vertNumb == 2) {
        result = FORel[0];
    }
    else if (vertNumb == 3) {
        double R[2][2];
        for (u_int u = 1; u < 3; u++) {
            for (u_int v = u + 1; v < 4; v++) {
                bool notEdge = true;
                for (u_int k = KAO[u - 1]; k < KAO[u]; k++) {
                    if (FO[k] == v) {   
                        notEdge = false;
                        SE = k;
                    }
                }
                if (notEdge) {
                    SE = edgNumb * 2;
                }
                if (SE < edgNumb * 2) {
                    R[u-1][v-2] = 1 - FORel[SE];
                }
                else { 
                    R[u-1][v-2] = 1;
                }  
            }
        }
        result = (R[0][0]*R[0][1]*R[1][1]+(1-R[0][0])*R[0][1]*R[1][1]+R[0][0]*(1-R[0][1])*R[1][1]+R[0][0]*R[0][1]*(1-R[1][1]));
    }
    else if (vertNumb == 4) {
        double a, b, c, d, e, f;
        double R[3][3];
        for (u_int u = 1; u < 4; u++) {
            for (u_int v = u + 1; v < 5; v++) {
                bool notEdge = true;
                for (u_int k = KAO[u - 1]; k < KAO[u]; k++) {
                    if (FO[k] == v) {   
                        notEdge = false;
                        SE = k;
                    }
                }
                if (notEdge) {
                    SE = edgNumb * 2;
                }
                if (SE < edgNumb * 2) {
                    R[u - 1][v - 2] = 1 - FORel[SE];
                }
                else { 
                    R[u - 1][v - 2] = 1;
                }  
            }
        }
        a = R[0][0]; b = R[1][1]; c = R[2][2]; d = R[0][2]; e = R[1][2]; f = R[0][1];
        result = 1 - (6*a*b*c*d*e*f - 2*(b*d*e*f*(a + c - 0.5) + a*c*e*f*(b + d - 0.5) + a*b*c*d*(e + f - 0.5)) + a*b*e + a*d*f + b*c*f + c*d*e);
    }
    else if (vertNumb == 5) {
        double a , b, c, d, e, f, q, h, k, l, 
                k1, k2, k3, k4, k5, k6, k7, k8, 
                k9, k10, k11, k12, k13, k14, k15;
        double R[4][4];
        for (u_int u = 1; u < 5; u++) {
            for (u_int v = u + 1; v < 6; v++) {
                bool notEdge = true;
                for (u_int k = KAO[u-1]; k < KAO[u]; k++) {
                    if (FO[k] == v) {   
                        notEdge = false;
                        SE = k;
                    }
                }
                if (notEdge) {
                    SE = edgNumb * 2;
                }
                if (SE < (edgNumb * 2)) {
                    R[u-1][v-2] = 1 - FORel[SE];
                }
                else { 
                    R[u-1][v-2] = 1;
                }  
            }
        }
        a = R[0][0]; b = R[0][1]; c = R[0][2]; d = R[0][3]; e = R[1][1]; 
        f = R[1][2]; q = R[1][3]; h = R[2][2]; k = R[2][3]; l = R[3][3];
        k1 = 1 - e*(f*q + h*k);
        k2 = 1 - h*(b*k + c*l);
        k3 = 1 - l*(c*f + d*q);
        k4 = 1 - d*(a*b + q*k);
        k5 = 1 - a*(b*c + e*f);
        k6 = (1-a)*(1-h)*(1-k) + (1-a)*(1-l)*((1-h)*k+h*(1-k)) + a*h*k*(1-4*l);
        k7 = (1-c)*(1-d)*(1-e) + (1-e)*(1-l)*((1-c)*d+c*(1-d)) + d*e*l;
        k8 = (1-a)*(1-d)*(1-h) + (1-q)*(1-h)*((1-a)*d+a*(1-d)) + a*h*q;
        k9 = (1-a)*(1-b)*(1-l) + (1-e)*(1-l)*((1-a)*b+a*(1-b)) + a*e*l;
        k10 = (1-e)*(1-d)*(1-f) + (1-d)*(1-h)*((1-e)*f+e*(1-f)) + d*f*h;
        k11 = (1-b)*(1-f)*(1-q) + (1-b)*(1-l)*((1-f)*q+f*(1-q)) + b*q*l;
        k12 = (1-c)*(1-e)*(1-q) + (1-c)*(1-k)*((1-e)*q+e*(1-q)) + c*e*k;
        k13 = (1-b)*(1-d)*(1-f) + (1-f)*(1-k)*((1-b)*d+b*(1-d)) + b*d*f;
        k14 = (1-b)*(1-c)*(1-q) + (1-q)*(1-h)*((1-b)*c+b*(1-c)) + b*c*q;
        k15 = (1-a)*(1-c)*(1-k) + (1-f)*(1-k)*((1-a)*c+a*(1-c)) + c*f*k;

        result =  1 - (b*c*(a*d*k1+f*e*(d*q*k6+k*l*k8)) +
                    f*q*(a*e*k2+h*k*(a*b*k7+c*d*k9)) +
                    b*h*(e*k*k3+d*l*(a*f*k12+e*q*k15)) +
                    c*l*(f*h*k4+a*q*(b*k*k10+e*h*k13)) +
                    d*k*(q*l*k5+a*e*(c*h*k11+f*l*k14)));
    }

    return result;
}

u_int Graph::getVertNumb() const {
    return vertNumb;
}

u_int Graph::getEdgNumb() const {
    return edgNumb;
}

u_int Graph::deg(u_int i) const {
    //if ((i == 0) || (i > edgNumb)) {
    if ((i == 0) || (i > vertNumb)) {
        return 0;       //raise exception here
    }
    return (KAO[i] - KAO[i - 1]);
}


//my funcs ------------------------
u_int* Graph::getKAO() {
    return KAO;
}

u_int* Graph::getFO() {
    return FO;
}



}//end of math namespace 
