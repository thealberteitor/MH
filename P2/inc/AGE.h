#ifndef AGE_H
#define AGE_H

#include <iostream>
#include <vector>

using namespace std;

class poblacion{

  public:
    //Variables Algoritmo Genético
    int n;
    vector<vector< double> > dist;
    vector< vector<bool> > soluciones;
    vector<double> values;
    int best;

    //Variables de cruce y mutación
    double mut;
    //double prob_cruce;

    //Por comodidad declaramos las funciones public

    //constructor, le pasamos la matriz de distancias
    poblacion(vector<vector<double> > distances);

    //Evalua la población, inserta en best el mejor de todos
    void evaluarPoblacion();

    //printea los costes de toda la población
    void mostrarPoblacion();

    //Muestra el mejor de la población, actualiza el índice de 'best'
    void mejor_indice();

    //Ordena la población según los costes
    void ordenar();

    //Muta la solución pasada por argumento.
    void mutarHijo(vector<bool> &h1);

    //Función de mutación. Recorre toda la población y muta cada gen
    //con probabilidad 'mut'
    void OperadorMutacion(pair< vector<bool>,vector<bool> > &hijos);

    //Torneo binario entre dos soluciones, gana el mejor
    int TorneoBinario(int sol1, int sol2);

    //Operador de seleccion
    pair< int,int > seleccion();

    //Cruce Uniforme, necesita reparación, devuelve 2 hijos
    pair< vector<bool>,vector<bool> > cruceUniforme(int m, pair<int,int> padres);

    //Cruce posicional, devuelve 2 hijos
    pair< vector<bool>,vector<bool> > crucePosicional(pair<int,int> padres);

    //Reemplamiento con elitismo
    void reemplazamiento(pair< vector<bool>,vector<bool> > hijos, double c1, double c2);

};


#endif
