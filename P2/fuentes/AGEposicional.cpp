#include <iostream>
#include <fstream>
#include <vector>
#include "time.h"
#include <utility> //pair
#include<algorithm>
#include<unistd.h>
#include <chrono>
#include <random>
#include "../inc/AGE.h"

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
  mut=0.001;
  soluciones={};
  dist=distances;
  best=0;
}


void poblacion::evaluarPoblacion(){
  values.clear();
  for(int i=0; i<n; i++)
    values.push_back(CosteEstimado(soluciones[i],dist));

  best = distance(values.begin(), max_element(values.begin(), values.end()));
}



void poblacion::mostrarPoblacion(){
  values.clear();
  evaluarPoblacion();

  for(int i=0; i<n; i++)
    cout << values[i]<< " ";

}


void poblacion::mejor_indice(){
  best = distance(values.begin(), max_element(values.begin(), values.end()));
  //cout << values[best];
}



void poblacion::ordenar(){
  sort(values.begin(), values.end());
}



void poblacion::mutarHijo(vector<bool> &h1){
  int p1;
  for(int i=0; i<h1.size(); i++){
    if(genAleat(mut)){
      do{
        p1=rand()%h1.size();
      }while(h1[p1]==h1[i]);

      swap(h1[p1], h1[i]);
    }
  }
}



/*
void poblacion::mutarHijo(vector<bool> &h1){
  int p1,p2;
  int total=h1.size()*mut;

  while(total>0){
    p1=rand()%h1.size();

    do{
      p2 = rand()%h1.size();
    }while(h1[p1]==h1[p2]);
    total--;
  }
  swap(h1[p1], h1[p2]);
}*/




void poblacion::OperadorMutacion(pair< vector<bool>,vector<bool> > &hijos){
  mutarHijo(hijos.first);
  mutarHijo(hijos.second);
}



int poblacion::TorneoBinario(int sol1, int sol2){
  return (values[sol1]>values[sol2]) ? sol1 : sol2;
}


pair< int,int > poblacion::seleccion(){
  pair<int,int> padres;
  do{
    padres.first = TorneoBinario(rand()%n, rand()%n);
    padres.second = TorneoBinario(rand()%n, rand()%n);
  }while(padres.first==padres.second); //para favorecer la diversidad exigimos que sean diferentes.

  return padres;
}





pair< vector<bool>,vector<bool> > poblacion::crucePosicional(pair<int,int> padres){
  vector<int> hijo1(dist.size());
  vector<int> hijo2(dist.size());

  vector<bool> p1,p2;

  hijo1.clear();
  hijo2.clear();


  int r=rand()%2==0 ? padres.first : padres.second;


  for(int j=0; j<soluciones[padres.first].size(); j++){
    if(soluciones[padres.first][j]==soluciones[padres.second][j]){
      //si son iguales los relleno
      hijo1.push_back(soluciones[padres.first][j]);
      hijo2.push_back(soluciones[padres.first][j]);
    }
    else{
      //si no coincide le meto un -1 para después modificarlo
      hijo1.push_back(-1);
      hijo2.push_back(-1);

      //cojo los restos de un padre aleatorio
      p1.push_back(soluciones[r][j]);
      p2.push_back(soluciones[r][j]);
    }
  }
  random_shuffle(p1.begin(), p1.end());
  random_shuffle(p2.begin(), p2.end());

  //Relleno todos los -1
  for(int j=0; j<soluciones[padres.first].size(); j++){
    if(hijo1[j]==-1){
      hijo1[j]=p1.back();
      hijo2[j]=p2.back();

      p1.pop_back();
      p2.pop_back();
    }
  }

  pair< vector<bool>,vector<bool> > hijos(convertToBool(hijo1), convertToBool(hijo2));
  return hijos;
}



void poblacion::reemplazamiento(pair< vector<bool>,vector<bool> > hijos, double c1, double c2){
  int peor1 = distance(values.begin(), min_element(values.begin(), values.end()));
  if(values[peor1]<c1){
    soluciones[peor1]=hijos.first;
    values[peor1]=c1;
  }

  int peor2 = distance(values.begin(), min_element(values.begin(), values.end()));
  if(values[peor2]<c2){
    soluciones[peor2]=hijos.second;
    values[peor2]=c2;
  }

  //actualizo el mejor índice
  best = distance(values.begin(), max_element(values.begin(), values.end()));
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




void AGEposicional(int m, vector<vector<double> > dist){
  const int EVALUACIONES=100000;
  int n=dist.size();
  int eval=0;

  //INICIALIZAR LA POBLACIÓN
  poblacion poblacion1(dist);
  for(int i=0; i<poblacion1.n; i++)
    poblacion1.soluciones.push_back(SolucionInicial(n,m));

  pair<int,int> padres;
  pair< vector<bool>,vector<bool> > hijos;

  //EVALUAR LA POBLACIÓN
  poblacion1.evaluarPoblacion();

  while(eval<EVALUACIONES){

    //OPERADOR DE SELECCIÓN (TORNEO BINARIO) tantos torneos como elementos en población
    poblacion poblacion2(dist);
    padres=poblacion1.seleccion();


    //CRUCE POSICIONAL
    //siempre se cruzan los dos padres
    //hijos = poblacion1.cruceUniforme(m, padres);
    hijos=poblacion1.crucePosicional(padres);

    //MUTACIÓN, PROB de mutar un gen es de 0.001
    poblacion1.OperadorMutacion(hijos);

    //EVALUAR LA POBLACIÓN
    double coste1 = CosteEstimado(hijos.first, dist);
    double coste2 = CosteEstimado(hijos.first, dist);
    eval+=2;


    //ESQUEMA DE REEMPLAZAMIENTO:
    //Para conservar el elitismo: si la mejor solución de la generación anterior
    //no sobrevive, sustituye directamente la peor sol de la nueva población.
    poblacion1.reemplazamiento(hijos, coste1, coste2);


    //cout << "EVAL: "   << eval <<endl;
    //cout << "MEJOR: " << poblacion1.values[poblacion1.best]<<endl;
  }
  cout << "MEJOR: " << poblacion1.values[poblacion1.best];

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
    AGEposicional(m, distancias);
    double total = clock() - start_time;

    //cout << "Ejecutando: " << archivo <<endl;
    cout << endl << "Tiempo: " << total/CLOCKS_PER_SEC <<endl;

    entrada.close();
  }else
      cout << "No se puede abrir el fichero " << argv[1] <<endl;
}
