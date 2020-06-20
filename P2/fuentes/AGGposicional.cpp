#include <iostream>
#include <fstream>
#include <vector>
#include "time.h"
#include <utility> //pair
#include<algorithm>
#include<unistd.h>
#include <chrono>
#include <random>
#include "../inc/AGG.h"

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
bool genAleat(double prob, unsigned seed = rand()){
  static std::mt19937 gen{seed};
  std::uniform_real_distribution<> dist;
  return dist(gen) < prob;
}



vector<bool> reparacion(vector<bool> todos, int m, vector<vector<double> > dist){
  int num_1=0, v=0;
  vector<double> contribuciones;
  int indice;
  double mejor;

  for(int i=0; i<todos.size(); i++){
    if(todos[i])
      num_1++;
  }

  v = m-num_1;

  do{
    if(v==0)
      return todos;

    if(v<0){
    //quitar los que más contribución aporten
      mejor=0;
      indice=0;
      for(int i=0; i<todos.size();i++){
        if(todos[i]){
          if(ContribucionIndep(i,todos,dist)>mejor){
            mejor=ContribucionIndep(i,todos,dist);
            indice = i;
          }
        }
      }
      todos[indice]=false;
      v++;

    }else{
      //añadir los que más contrib aporten
      mejor=0;
      indice=0;
      for(int i=0; i<todos.size();i++){
        if(!todos[i]){
          if(ContribucionIndep(i,todos,dist)>mejor){
            mejor=ContribucionIndep(i,todos,dist);
            indice = i;
          }
        }
      }

      int random;
      do{
        random = rand()%todos.size();
      }while(todos[random]==true);

      todos[random]=indice;
      v--;
    }

  }while(v!=m);

  return todos;
}



vector<bool> convertToBool(vector<int> hijo1){
  vector<bool> h1;
  for(int i=0; i<hijo1.size(); i++){
    if(hijo1[i]==1)
      h1.push_back(true);
    else
      h1.push_back(false);
  }
  return h1;
}


//constructor, le pasamos la matriz de distancias
poblacion::poblacion(vector<vector<double> > distances){
  n=50;
  mut = 0.001;
  prob_cruce=0.7;
  soluciones={};
  dist=distances;
  best=0;
}


void poblacion::evaluarPoblacion(int &eval){
  values.clear();
  for(int i=0; i<n; i++){
    values.push_back(CosteEstimado(soluciones[i],dist));
  }
  eval+=n;
  best = distance(values.begin(), max_element(values.begin(), values.end()));
}



void poblacion::mostrarPoblacion(){
  values.clear();
  //evaluarPoblacion(eval);

  for(int i=0; i<n; i++){
    cout << values[i]<< " ";
  }
}



void poblacion::mejor_indice(){
  best = distance(values.begin(), max_element(values.begin(), values.end()));
  cout << values[best];
}



void poblacion::ordenar(){
  sort(values.begin(), values.end());
}


//Usamos esta función para el ejercicio EXTRA, para el resto de ejecuciones
//usamos la que está comentada justo abajo ya que usa la esperanza
//matemática y es mucho más eficiente.
/*
void poblacion::mutacion(int &num_mutaciones){
  int p1;
  for(int i=0; i<n; i++){
    for(int j=0; j<soluciones[i].size(); j++){
      if(genAleat(mut)){
        do{
          p1=rand()%soluciones[i].size();
        }while(soluciones[i][p1]==soluciones[i][j]);

        swap(soluciones[i][p1], soluciones[i][j]);
        num_mutaciones++;
      }
    }
  }
}
*/



void poblacion::mutacion(){

  int total = mut * n * dist.size();
  int n1, m1, m2;
  while(total>0){
    n1 = rand()%n;
    m1 = rand()%dist.size();

    do{
      m2 = rand()%dist.size();
    }while(soluciones[n1][m1]==soluciones[n1][m2]);

    swap(soluciones[n1][m1],soluciones[n1][m2]);
    total--;
  }
}





int poblacion::TorneoBinario(int sol1, int sol2){
  return (values[sol1]>values[sol2]) ? sol1 : sol2;
}



poblacion poblacion::seleccion(){
  poblacion poblacion2(dist);

  int ganador;
  for(int i=0; i<n; i++){
    ganador = TorneoBinario(rand()%n, rand()%n);
    poblacion2.soluciones.push_back(soluciones[ganador]);
  }
  return poblacion2;
}





void poblacion::crucePosicional(){
  vector<int> hijo1(dist.size());
  vector<int> hijo2(dist.size());

  int cruces=prob_cruce*dist.size()/2;

  int i1,i2,r;
  vector<bool> p1,p2;

  while(cruces>0){

    p1.clear();
    p2.clear();
    hijo1.clear();
    hijo2.clear();

    //Combinamos parejas consecutivas
    i1=rand()%(n-1);
    i2=i1+1;

    r=rand()%2==0 ? i1 : i2;

    for(int j=0; j<soluciones[i1].size(); j++){
      if(soluciones[i1][j]==soluciones[i2][j]){
        //si son iguales los relleno
        hijo1.push_back(soluciones[i1][j]);
        hijo2.push_back(soluciones[i1][j]);
      }
      else{
        //si no coincide le meto un -1 para después modificarlo
        hijo1.push_back(-1);
        hijo2.push_back(-1);

        //cojo los restos de un padre
        p1.push_back(soluciones[r][j]);
        p2.push_back(soluciones[r][j]);
      }
    }
    random_shuffle(p1.begin(), p1.end());
    random_shuffle(p2.begin(), p2.end());

    //Relleno todos los -1
    for(int j=0; j<soluciones[i1].size(); j++){
      if(hijo1[j]==-1){
        hijo1[j]=p1.back();
        hijo2[j]=p2.back();

        p1.pop_back();
        p2.pop_back();
      }
    }

    soluciones[i1]=convertToBool(hijo1);
    soluciones[i2]=convertToBool(hijo2);

    cruces--;
  }
}



/*
void poblacion::crucePosicional(){
  vector<int> hijo1(dist.size());
  vector<int> hijo2(dist.size());

  vector<bool> p1,p2;
  int r;

  for(int i=0; i<n; i+=2){
    if(genAleat(prob_cruce)){

      r=rand()%2==0 ? i : i+1;

      p1.clear();
      p2.clear();
      hijo1.clear();
      hijo2.clear();

      for(int j=0; j<soluciones[i].size(); j++){
        if(soluciones[i][j]==soluciones[i+1][j]){
          //si son iguales los relleno
          hijo1.push_back(soluciones[i][j]);
          hijo2.push_back(soluciones[i][j]);
        }
        else{
          //si no coincide le meto un -1 para después modificarlo
          hijo1.push_back(-1);
          hijo2.push_back(-1);

          //cojo los restos de un padre, r
          p1.push_back(soluciones[r][j]);
          p2.push_back(soluciones[r][j]);
        }
      }
      random_shuffle(p1.begin(), p1.end());
      random_shuffle(p2.begin(), p2.end());

      //Relleno todos los -1
      for(int j=0; j<soluciones[i].size(); j++){
        if(hijo1[j]==-1){
          hijo1[j]=p1.back();
          hijo2[j]=p2.back();

          p1.pop_back();
          p2.pop_back();
        }
      }
      soluciones[i]=convertToBool(hijo1);
      soluciones[i+1]=convertToBool(hijo2);
    }
  }
}
*/


void poblacion::reemplazamiento(poblacion &poblacion2){

  if(find(poblacion2.values.begin(),poblacion2.values.end(),values[best])==poblacion2.values.end()){
    //Si no está el mejor de la población anterior:
    int peor=distance(poblacion2.values.begin(), min_element(poblacion2.values.begin(), poblacion2.values.end()));
    poblacion2.soluciones[peor]=soluciones[best];
    poblacion2.values[peor]=values[best];

    if(poblacion2.values[peor]>poblacion2.values[poblacion2.best])
      poblacion2.best = peor;
  }
  //La población de hijos sustituye a la actual
  *this=poblacion2;
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




void AGGposicional(int m, vector<vector<double> > dist){
  const int EVALUACIONES=100000;
  int n=dist.size();
  int eval=0;

  //INICIALIZAR LA POBLACIÓN
  poblacion poblacion1(dist);
  for(int i=0; i<poblacion1.n; i++)
    poblacion1.soluciones.push_back(SolucionInicial(n,m));


  //EVALUAR LA POBLACIÓN
  poblacion1.evaluarPoblacion(eval);

  poblacion poblacion2(dist);
  while(eval<EVALUACIONES){

    //OPERADOR DE SELECCIÓN (TORNEO BINARIO) tantos torneos como elementos en población
    poblacion2 = poblacion1.seleccion();

    //CrucePosicional
    poblacion2.crucePosicional();

    //MUTACIÓN, PROB de mutar un gen es de 0.001
    poblacion2.mutacion();

    //EVALUAR LA POBLACIÓN
    poblacion2.evaluarPoblacion(eval);

    //ESQUEMA DE REEMPLAZAMIENTO:
    //Para conservar el elitismo: si la mejor solución de la generación anterior
    //no sobrevive, sustituye directamente la peor sol de la nueva población.
    poblacion1.reemplazamiento(poblacion2);

    //cout << "EVAL: "   << eval <<endl;
    //cout << "MEJOR: " << poblacion1.values[poblacion1.best]<<endl;
  }
  cout << "MEJOR: " << poblacion1.values[poblacion1.best]<<endl;
}





int main(int argc, char *argv[]){

  string s, line, archivo = argv[2];
  int semilla = atoi(argv[1]);
  ifstream entrada(archivo);

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
    AGGposicional(m, distancias);
    double total = clock() - start_time;

    //cout << "Ejecutando: " << archivo <<endl;
    cout << endl << "Tiempo: " << total/CLOCKS_PER_SEC <<endl;

    entrada.close();
  }else
      cout << "No se puede abrir el fichero " << argv[1] <<endl;
}
