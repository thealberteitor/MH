#ifndef AM_H
#define AM_H

#include <iostream>
#include <vector>

using namespace std;

class poblacion{

  public:
    //Variables Algoritmo Genético
    int n;
    vector<vector< double> > dist;
    vector< vector<bool> > soluciones;
    int tipo;
    vector<double> values;
    int best;

    //parámetros mutación y cruce
    double prob_ls; //Algoritmo memético
    double mut;
    double prob_cruce;

    //Por comodidad declaramos las funciones public

    //constructor, le pasamos la matriz de distancias
    poblacion(vector<vector<double> > distances);


    //poblacion& operator=(const poblacion &orig);

    //Evalua la población, inserta en best el mejor de todos
    void evaluarPoblacion(int &eval);

    //printea los costes de toda la población
    void mostrarPoblacion();

    //Muestra el mejor de la población, actualiza el índice de 'best'
    void mejor_indice();

    //Ordena la población según los costes
    void ordenar();

    //Pasar a Enteros
    vector< vector<int> > pasarAEnteros();

    //Pasar a Codificación binaria
    void pasarABinarios(vector< vector<int> > entera);

    //Para el algoritmo memético, aplicar BL de la P1
    void memetico(int &eval);

    //Función de mutación. Recorre toda la población y muta cada gen
    //con probabilidad 'mut'
    void mutacion();

    //Torneo binario entre dos soluciones, gana el mejor
    int TorneoBinario(int sol1, int sol2);

    //Operador de seleccion
    poblacion seleccion();

    //Cruce Uniforme, necesita reparación
    void cruceUniforme(int m);

    //Cruce posicional
    void crucePosicional();

    //Reemplamiento con elitismo
    void reemplazamiento(poblacion &poblacion2);


};


#endif
