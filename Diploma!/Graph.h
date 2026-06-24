#ifndef SRC_GRAPH_H
#define SRC_GRAPH_H

#include <fstream>

#include "base_def.h"
#include <vector>

namespace math {

/**
 * KAO-FO реализация неориентированного графа без петель
 * Граф заполняется данными через чтение из файла, 
 * либо через GraphGenerator (пока его возможности ограничены полными графами, решетками и сетью GEANT)
*/
class Graph {
    friend class GraphFactory;

    u_int *KAO;     //массив сумм степеней вершин (нумерация вершин идёт с 1). KAO[0] = 0. deg(i) = KAO[i] - KAO[i - 1], i = 1, ..., n
    u_int *FO;      //массив, где элементы от FO[KAO[i-1]] до FO[KAO[i]] - вершины, смежные с вершиной i
    double *FORel;  //массив вероятностей присутствия рёбер в графе. Вероятность FORel[j] соответствует ребру FO[j] (по умолчанию равна 0.75) 

    u_int vertNumb; //количество вершин в графе
    u_int edgNumb;  //количество рёбер в графе
    
    std::vector<u_int> targets; //vector of target vertexes

    std::vector<int> firstTriangle;


    /**
     * Очистка памяти
    */
    void memclear();

    /**
     * Копирование данных
     * @param other Граф, откуда происходит копирование
    */
    void memcopy(const Graph &other);

#if __cplusplus >= 201103L
    /**
     * Перемещение данных
     * @param other Граф, откуда происходит перемещение
    */
    void memmove(Graph &&other);
#endif

    /**
     * Выделение памяти в newKAO - размера (_vertNumb + 1), в newFO, newFORel - размера (2 * _edgNumb).
    */
    void memallocNewGraph(u_int _vertNumb, u_int _edgNumb, u_int *&newKAO, u_int *&newFO, double *&newFORel) const;

    /**
     * Изменение данных графа без реаллоцирования памяти
     * @param vertLim Предел количества вершин, до которого переписывается KAO
     * @param edgLim  Предел количества ребер, до которого переписываются FO, FORel
     * @param _KAO    Указатель на массив, откуда копируются степени вершин
     * @param _FO     Указатель на массив, откуда копируются связанные вершины
     * @param _FORel  Указатель на массив, откуда копируются вероятности связанных вершин
    */
    void memupdate(u_int vertLim, u_int edgLim, u_int *&_KAO, u_int *&_FO, double *&_FORel);

    /**
     * Изменение данных графа c реаллоцированием памяти
     * @param vertLim Предел количества вершин, до которого переписывается KAO
     * @param edgLim  Предел количества ребер, до которого переписываются FO, FORel
     * @param _KAO    Указатель на массив, откуда копируются степени вершин
     * @param _FO     Указатель на массив, откуда копируются связанные вершины
     * @param _FORel  Указатель на массив, откуда копируются вероятности связанных вершин
    */
    void memrebase(u_int vertLim, u_int edgLim, u_int *&_KAO, u_int *&_FO, double *&_FORel);

    /**
     * Проверка, выделена ли память у графа
    */
    bool meminited() const;

    /**
     * Проверка, является ли пара (u, v) ребром графа
     * @param u Вершина
     * @param v Вершина
    */
    bool isEdge(u_int u, u_int v) const;

    /**
     * Проверка, является ли пара (u, v) ребром графа
     * @param u Вершина
     * @param v Вершина
     * @param pos Позиция в массиве FO, на которой находится ребро
    */
    bool isEdgeWithPos(u_int u, u_int v, u_int &pos) const;

public:
    Graph();


#if __cplusplus >= 201103L
    Graph(Graph &&other);
#endif

    Graph(const Graph &graph);

    Graph(u_int _vertNumb, u_int _edgNumb, u_int *&_KAO, u_int *&_FO, double *&_FORel);

    ~Graph();

    friend std::ifstream &operator >>(std::ifstream &input, Graph &graph);

    //массив сумм степеней вершин (нумерация вершин идёт с 1). KAO[0] = 0. deg(i) = KAO[i] - KAO[i - 1], i = 1, ..., n
   
    u_int* getFO();
    u_int* getKAO();

    /**
     * Запись в исходный граф данных из передаваемого графа 
    */
    Graph &operator =(const Graph &graph);

    Graph &operator =(const Graph *graph);

#if __cplusplus >= 201103L
    Graph &operator =(Graph &&other);
#endif

    /**
     * Стягивание ребра (u, v), если оно присутствует в графе. 
     * Если существует ребро, инцидентное обеим вершинам, оно стягивается в одно с пересчётом вероятности его присутствия в графе
     * @param u Вершина
     * @param v Вершина
     * @return Граф со стянутым ребром
    */
    Graph *mergeEdge(u_int u, u_int v) const;

    /**
     * Удаление ребра (u, v), если оно присутствует в графе.
     * @param u Вершина
     * @param v Вершина
     * @return Граф с удаленным ребром
    */
    Graph *cutEdge(u_int u, u_int v) const;

    /**
     * Удаление висячих вершин (т.е. вершин степени 1)
     * @param prob Вероятность, на которую заменяются все висячие вершины
    */
    void shootsDeletion(double &prob);

    /**
     * Последовательно-параллельное преобразование
     * Удаление вершин степени 2, т.е замена ребер (u, v) и (u, w) на ребро (v, w). 
     * Если ребро (v, w) уже существует, происходит дополнительный пересчёт вероятности его присутствия в графе
     * @param pstFactor Вероятность, на которую заменяются вершины степени 2
    */
    void parallelSeriesTransformation(double &pstFactor);

    /**
     * Проверка графа на связность
    */
    bool isConnective() const;

    /**
     * Расчёт вероятности всетерминальной связности для графа на 5 и менее вершинах
     * @return Вероятность связности
    */
    double baseProbabilities() const;

    /**
     * @return Количество вершин в графе
    */
    u_int getVertNumb() const;

    /**
     * @return Количество рёбер в графе
    */
    u_int getEdgNumb() const;

    /**
     * @param i номер вершины
     * @return Степень вершины
    */
    u_int deg(u_int i) const;


};//Graph



}//math



#endif