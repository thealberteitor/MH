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
#include <cfloat>

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





vector<int> pasarAEnteros(vector<bool> solucion){
  vector<int> sel_i;

  for(int i=0; i<solucion.size(); i++){
    if(solucion[i]==true)
      sel_i.push_back(i);
  }
  return sel_i;
}



vector<bool> pasarABinarios(vector<int> solucion, int n){
  vector<bool> sel_b(n);

  for(int i=0; i<solucion.size(); i++){
    sel_b[i]=false;
  }

  for(int i=0; i<solucion.size();i++){
      sel_b[solucion[i]]=true;
  }
  return sel_b;
}







//////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//Búsqueda Local

//tenemos las contribuciones independientes, tomar la que tenga menos contribución
double ContribucionIndepBL(int indice, vector<int> &sel, vector<vector<double> > &dist){
  vector<int>::iterator it;
  double sum=0;

  for(it=sel.begin(); it!=sel.end(); it++)
    sum += dist[indice][*it];

  return sum; //mirar seminario2, pag 24
}



double CosteEstimadoBL(vector<int> entrada, vector<vector<double> > distancias){
  double zms=0;
  vector<int>::iterator it;
  vector<int>::iterator it2;

  for(it=entrada.begin(); it!= entrada.end(); it++)
      zms+= ContribucionIndepBL(*it, entrada, distancias);

  return zms;
}


int GeneraJ(int i, vector<int> sel, int n, vector<int> &todasJ){
  int j;
  vector<int>::iterator it, it2;

  do{
    j = rand()%n;
    it = find(sel.begin(), sel.end(), j);
    it2 = find(todasJ.begin(), todasJ.end(), j);

    //Si j está en seleccionados o j ya había salido antes repetimos
  }while(it != sel.end() || it2!=todasJ.end());

  todasJ.push_back(j); //Lo insertamos para no volver a insertarlo nuevamente
  return j;
}


pair< vector<bool>, double > EvaluaVecinos(pair< vector<bool>, double > f1, vector<vector<double> > &dist){


  bool mejora = true, salir;
  int n = dist.size(), c, ind;
  int eval=0;

  const int MAX = 10000;
  pair< vector<int>, double > inicial(pasarAEnteros(f1.first), f1.second);
  int m = inicial.first.size();

  vector<int>::iterator it;
  vector<double>::iterator minimo;
  vector<int> todasJ;
  vector<double> contribuciones;
  pair< vector<bool> , double> dev;

  int siguienteMin=0;
  double nueva, antigua, j, coste;


  while(eval<MAX && mejora){ //condición de parada
    //Calculamos el mínimo del vector de contribuciones
    if(todasJ.size()==0){  //si el intercambio anterior fue aceptado
      for(it=inicial.first.begin(); it!=inicial.first.end(); it++)
        contribuciones.push_back(ContribucionIndepBL(*it, inicial.first, dist));

      minimo = min_element(contribuciones.begin(), contribuciones.end());
      ind = distance(contribuciones.begin(), minimo);

      siguienteMin=0;
    }
    else if(todasJ.size()!=0 && siguienteMin<m){//Si se han generado todos los j y no está el entorno explorado
      siguienteMin++;
      contribuciones[ind]=DBL_MAX;
      todasJ.clear();

      //Seleccionamos el siguiente mejor elemento
      minimo = min_element(contribuciones.begin(), contribuciones.end());
      ind = distance(contribuciones.begin(), minimo);

    }else if(siguienteMin==m){ //entorno explorado
      mejora = false;
      dev.first = pasarABinarios(inicial.first, dist.size());
      dev.second = CosteEstimado(dev.first, dist);
      return dev;
    }

    c = 0;
    salir=false;

    while(c< (n-m) && !salir){

      //Generamos un valor j que no se haya generado antes
      j = GeneraJ(ind, inicial.first, n, todasJ);

      //Calculamos los costes
      antigua = ContribucionIndepBL(inicial.first[ind], inicial.first, dist);
      nueva = ContribucionIndepBL(j, inicial.first, dist)-dist[j][ind];


      //if(CosteSolGeneral(aux,dist)-CosteSolGeneral(inicial.first,dist)>0){
      if(nueva>antigua){
        inicial.first[ind] = j;  //intercambio i por j
        inicial.second = inicial.second + nueva - antigua;; //actualizamos coste total (factorizado)

        salir=true;
        todasJ.clear();
        contribuciones.clear();
      }
      c++;
      eval++; //evaluaciones de la Búsqueda Local
    }
  }
  dev.first = pasarABinarios(inicial.first, dist.size());
  dev.second = CosteEstimado(dev.first, dist);
  return dev;
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




vector<bool> OperadorMutacion(vector<bool> sel, int m){
  int n=sel.size();
  int total =  m*0.1;
  int i, j;

  //Cambio brusco
  while(total>0){
    i = rand()%n;

    do{
      j = rand()%n;
    }while(sel[i]==sel[j]);

    swap(sel[i],sel[j]);
    total--;
  }
  return sel;
}




void BusquedaLocalReiterada(int m, vector<vector<double> > dist){
  int totalBL=10;
  vector<bool> ini = SolucionInicial(dist.size(),m);
  pair< vector<bool>, double > sel(ini, CosteEstimado(ini,dist) );

  sel = EvaluaVecinos(sel, dist);
  totalBL--;
  pair< vector<bool>, double> mejor_sol(sel.first, sel.second);

  while(totalBL>0){
    //cerr << "Coste mejor: " << totalBL << " " << mejor_sol.second << endl;

    sel.first = OperadorMutacion(sel.first, m);
    sel.second = CosteEstimado(sel.first, dist);
    sel = EvaluaVecinos(sel, dist);
    //cerr << "Mejor que el anterior: " << totalBL << " " << sel.second << endl;

    totalBL--;

    if(sel.second>mejor_sol.second)
      mejor_sol=sel;
    else
      sel=mejor_sol;
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
    BusquedaLocalReiterada(m, distancias);
    double total = clock() - start_time;

    //cout << "Ejecutando: " << archivo <<endl;
    cout << endl << "Tiempo: " << total/CLOCKS_PER_SEC <<endl;

    entrada.close();
  }else
      cout << "No se puede abrir el fichero " << argv[1] <<endl;
}
