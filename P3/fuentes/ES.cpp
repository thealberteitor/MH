#include <iostream>
#include <fstream>
#include <vector>
#include "time.h"
#include <utility> //pair
#include<algorithm>
#include<unistd.h>
#include <chrono>
#include <random>
#include<cassert>

using namespace std;


//tenemos las contribuciones independientes, tomar la que tenga menos contribución
double ContribucionIndep(int indice, vector<bool> &sel, vector<vector<double> > &dist){
  double sum=0;
  for(int i=0; i<sel.size(); i++)
    sum += sel[i]*dist[indice][i];

  return sum; //mirar seminario2, pag 24
}



//de la forma anterior (greedy no se puede hacer)
//Ya no se puede hacer así, ya que necesitamos calcular las contribuciones
//de forma independiente y así seleccionar los elementos que menos contribuyen
double CosteEstimado(vector<bool> entrada, vector<vector<double> > distancias){
  double zms=0;

  for(int i=0; i<entrada.size(); i++){
    if(entrada[i]==true)
      zms+= ContribucionIndep(i, entrada, distancias);
  }
  return zms/2;
}


//bool genAleat(double prob, unsigned seed = std::random_device{}())
//double U(double prob, unsigned seed = rand()){
double U(unsigned seed = rand()){

  static std::mt19937 gen{seed};
  std::uniform_real_distribution<> dist;
  return dist(gen); //< prob;
}



vector<bool> Intercambio(vector<bool> h1, int &intercambio_i, int &intercambio_j){
  int i;
  int j;

  do{
    i=rand()%h1.size();
  }while(h1[i]==false);

  do{
    j=rand()%h1.size();
  }while(h1[i]==h1[j]);

  swap(h1[i], h1[j]);

  intercambio_i=i;
  intercambio_j=j;

  return h1;
}



//pasamos a una representación binaria. 1 si está cogido y 0 si no lo está.
vector<bool> SolucionInicial(int n, int m){
  vector<bool> sel(n);
  int random;
  int m_gen;

  do{
    m_gen=0;
    random =rand()%n;
    sel[random]=true;

    for(int i=0; i<n; i++){
      if(sel[i]==true)
        m_gen++;
    }
  }while(m_gen!=m);

  return sel;
}


void EnfriamientoSimulado(int m, vector<vector<double> > dist){
  const int MAX_VECINOS=10*dist.size();
  const int MAX_EXITOS=0.1*MAX_VECINOS;
  const int EVALUACIONES=100000;
  const int ENFRIAMIENTOS=100000/MAX_VECINOS;

  vector<bool> sel = SolucionInicial(dist.size(),m);
  pair<vector<bool>, double> mejor_sol;

  double diferencia, antiguo, nuevo;
  int intercambio_i, intercambio_j;

  pair<vector<bool>, double> siguiente;
  siguiente.second=CosteEstimado(sel, dist);;

  double t0=(0.3*siguiente.second)/(-log(0.3));
  double tf=0.001;
  assert(tf<t0);

  double beta = (t0-tf)/(ENFRIAMIENTOS*t0*tf);
  int eval=0, exitos=1, enfriamientos=0;



  //while(tf<t0 && enfriamientos<ENFRIAMIENTOS){
  while(tf<t0 && eval<EVALUACIONES && exitos!=0){
    exitos=0;
    for(int i=0; i<MAX_VECINOS && exitos<MAX_EXITOS; i++){
      siguiente.first=Intercambio(sel, intercambio_i, intercambio_j);
      eval++;

      //Factorizando la función objetivo
      antiguo= ContribucionIndep(intercambio_i, sel, dist);
      nuevo = ContribucionIndep(intercambio_j, siguiente.first,dist);
      diferencia = nuevo - antiguo;

      if(diferencia>0  || U() < exp(diferencia/t0)){
        exitos++;
        sel=siguiente.first;

        siguiente.second=siguiente.second+diferencia;

        if(siguiente.second>mejor_sol.second){
          mejor_sol=siguiente;
        }
      }

    }
    t0 = t0/(1+beta*t0);
    enfriamientos++;
  }
  cout << CosteEstimado(mejor_sol.first, dist) << endl;
}





int main(int argc, char *argv[]){

  string s, line, archivo = argv[2];
  int semilla = atoi(argv[1]);
  ifstream entrada(archivo);

  //srand(time(NULL));
  srand(semilla);

  int n,m;

  //matriz de distancias entre elementos
  //Almacenamos toda la matriz, necesaria para despues calcular
  //las distancias acumuladas
  int i,j;
  double d;

  if(entrada.is_open()){
    cout << "--------------------------------------"<<endl;

    cout << "Fichero " << argv[2] << " abierto correctamente." << endl;

    //leemos n, m
    entrada >> n >> m;

    vector <double> interno (n,0);
    vector<vector<double> > distancias(n, interno);

    while(getline(entrada,line)){
      entrada >> i >> j >> d;
      distancias[i][j] = d;
      distancias[j][i] = d;
    }


    //llamar algoritmo con matriz de distancias y calculamos tiempo
    clock_t start_time = clock();
    EnfriamientoSimulado(m, distancias);
    double total = clock() - start_time;

    //cout << "Ejecutando: " << archivo <<endl;
    cout << endl << "Tiempo: " << total/CLOCKS_PER_SEC <<endl;

    entrada.close();
  }else
      cout << "No se puede abrir el fichero " << argv[1] <<endl;
}
