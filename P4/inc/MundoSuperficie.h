#ifndef MS_H
#define MS_H

#include <iostream>
#include <vector>

using namespace std;

class poblacion{

  public:

    int n;
    string superficie;
    vector<vector< double> > dist;
    vector< vector<bool> > soluciones;
    vector<double> values;
    int best;
    int id_k, id_j;
    vector<int> ones;


    //Por comodidad declaramos las funciones public

    //constructor, le pasamos la matriz de distancias
    poblacion(string nombre, int id_k, int id_j, vector<vector<double> > distances);

    //Evalua la población, inserta en best el mejor de todos
    void evaluarPoblacion(int &eval);

    //Movimiento aleatorio de nuestras soluciones en cada superficie
    void movimiento();

    //printea los costes de toda la población
    void mostrarPoblacion();

    //Muestra el mejor de la población, actualiza el índice de 'best'
    void mejor_indice();

    //Ordena la población según los costes
    void ordenar();

    //Come
    void nutrirse();

    //muda la piel cuando come mucho
    void mudar_piel();

    //aplicar BL
    void aplicar_BL(int &eval);

    //viajar de una superficie a otra para favorecer la exploración de otras zonas
    void viaje(poblacion bk);

    //Mutación brusca
    vector<bool> mutacionBrusca(vector<bool> sel);

    //Torneo binario entre dos soluciones, gana el mejor
    int TorneoBinario(int sol1, int sol2);

    //Operador de seleccion
    poblacion seleccion();

    void elitismo(vector<bool> mejor, double coste);

    //Reemplamiento con elitismo
    void reemplazamiento(poblacion &poblacion2);

};


#endif
