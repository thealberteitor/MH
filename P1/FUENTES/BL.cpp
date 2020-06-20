#include <iostream>
#include <fstream>
#include <vector>
#include "time.h"
#include <utility> //pair
#include<algorithm>
#include<bits/stdc++.h>
#include<unistd.h>
using namespace std;



//tenemos las contribuciones independientes, tomar la que tenga menos contribución
double ContribucionIndep(int indice, vector<int> &sel, vector<vector<double> > &dist){
  vector<int>::iterator it;
  double sum=0;

  for(it=sel.begin(); it!=sel.end(); it++)
    sum += dist[indice][*it];

  return sum; //mirar seminario2, pag 24
}



//de la forma anterior (greedy no se puede hacer)
//Ya no se puede hacer así, ya que necesitamos calcular las contribuciones
//de forma independiente y así seleccionar los elementos que menos contribuyen
double CosteEstimado(vector<int> entrada, vector<vector<double> > distancias){
  double zms=0;
  vector<int>::iterator it;
  vector<int>::iterator it2;

  for(it=entrada.begin(); it!= entrada.end(); it++)
      zms+= ContribucionIndep(*it, entrada, distancias);

  return zms;
}



//Equivale llamar a la función CosteEstimado
//Zms
double CosteSolGeneral(vector<int> entrada, vector<vector<double> > distancias){
  double zms=0;
  vector<int>::iterator it;
  vector<int>::iterator it2 ;

  for(it=entrada.begin() ; it!= entrada.end(); it++){
    for(it2=it+1; it2!=entrada.end(); it2++){
      zms+= distancias[*it][*it2];
    }
  }
  return zms;
}




vector<int> SolucionInicial2(int n, int m){

  vector<int> sel;
  vector<int>::iterator it;
  int random;

  while(sel.size()<m){
    random=rand()%n;
    //Si no está insertado
    it = find(sel.begin(), sel.end(), random);
    if(it == sel.end()){
      sel.push_back(random);
    }
  }
  return sel;
}


//en unordered set no se accede por el indice
vector<int> SolucionInicial(int n, int m){
  unordered_set<int> se;
  int random;

  while(se.size()<m){
    random=rand()%n;
    se.insert(random);
  }
  //convierto a vector
  vector<int> sel(se.begin(), se.end());
  return sel;
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



void EvaluaVecinos(pair<vector<int>, double > &inicial, vector<vector<double> > &dist){

  bool mejora = true, salir;
  int n = dist.size(), eval = 0, c, ind;
  int m = inicial.first.size();

  const int MAX = 100000;

  vector<int>::iterator it;
  vector<double>::iterator minimo;
  vector<int> todasJ;
  vector<double> contribuciones;

  int siguienteMin=0;
  double nueva, antigua,j, coste;


  while(eval<MAX && mejora){ //condición de parada
    //Calculamos el mínimo del vector de contribuciones
    if(todasJ.size()==0){  //si el intercambio anterior fue aceptado
      for(it=inicial.first.begin(); it!=inicial.first.end(); it++)
        contribuciones.push_back(ContribucionIndep(*it, inicial.first, dist));

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
      return;
    }

    c = 0;
    salir=false;

    while(c< (n-m) && !salir){

      //Generamos un valor j que no se haya generado antes
      j = GeneraJ(ind, inicial.first, n, todasJ);

      //Calculamos los costes
      antigua = ContribucionIndep(inicial.first[ind], inicial.first, dist);
      nueva = ContribucionIndep(j, inicial.first, dist)-dist[j][ind];


      //if(CosteSolGeneral(aux,dist)-CosteSolGeneral(inicial.first,dist)>0){
      if( nueva>antigua){
        inicial.first[ind]=j;  //intercambio i por j
        inicial.second = inicial.second + nueva - antigua;; //actualizamos coste total (factorizado)

        salir=true;
        todasJ.clear();
        contribuciones.clear();
      }
      c++;
      eval++;
    }
  }
}



void BusquedaLocal(int m, vector<vector<double> > dist){

  //almacenos los índices y su contribución
  pair<vector<int>, double> inicial;
  inicial.first = SolucionInicial(dist.size(), m);  //meto los m en sel
  inicial.second = CosteSolGeneral(inicial.first , dist);

  EvaluaVecinos(inicial, dist);


  cout << "COSTE SOLUCIÓN: " << CosteSolGeneral(inicial.first , dist) <<endl;
}



int main(int argc, char *argv[]){
  //ifstream entrada("MDG-b_9_n500_m50.txt");
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
    BusquedaLocal(m, distancias);
    double total = clock() - start_time;

    //cout << "Ejecutando: " << archivo <<endl;
    cout << "Tiempo: " << total/CLOCKS_PER_SEC <<endl;

    entrada.close();
  }else
      cout << "No se puede abrir el fichero " << argv[1] <<endl;
}
