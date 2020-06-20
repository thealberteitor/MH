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
  mut = 0.001;
  soluciones={};
  dist=distances;
  best=0;
}


void poblacion::evaluarPoblacion(){
  values.clear();
  for(int i=0; i<n; i++){
    values.push_back(CosteEstimado(soluciones[i],dist));
  }
  best = distance(values.begin(), max_element(values.begin(), values.end()));
}


void poblacion::mostrarPoblacion(){
  values.clear();
  evaluarPoblacion();

  for(int i=0; i<n; i++){
    cout << values[i]<< " ";
  }
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
}
*/



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





pair< vector<bool>,vector<bool> > poblacion::cruceUniforme(int m, pair<int,int> padres){
  vector<bool> hijo1(dist.size());
  vector<bool> hijo2(dist.size());

  int r=rand()%2==0 ? padres.first : padres.second;
  int s=rand()%2==0 ? padres.first : padres.second;

  for(int i=0; i<soluciones[padres.first].size(); i++){
    if(soluciones[padres.first][i]==soluciones[padres.second][i]){
      hijo1[i]=soluciones[padres.first][i];
      hijo2[i]=soluciones[padres.first][i];
    }
    else{
      hijo1[i]=soluciones[r][i];
      hijo2[i]=soluciones[s][i];
    }
  }
  //REPARADOR A HIJOS
  hijo1 = reparacion(hijo1,m, dist);
  hijo2 = reparacion(hijo2,m, dist);

  pair< vector<bool>,vector<bool> > hijos(hijo1,hijo2);
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




void AGEuniforme(int m, vector<vector<double> > dist){
  const int EVALUACIONES=100000;
  int n=dist.size();
  int eval=0;
  double coste1, coste2;
  //INICIALIZAR LA POBLACIÓN
  poblacion poblacion1(dist);
  for(int i=0; i<poblacion1.n; i++)
    poblacion1.soluciones.push_back(SolucionInicial(n,m));

  pair<int,int> padres;
  pair< vector<bool>,vector<bool> > hijos;

  //EVALUAR LA POBLACIÓN
  poblacion1.evaluarPoblacion();
  poblacion poblacion2(dist);

  while(eval<EVALUACIONES){

    //OPERADOR DE SELECCIÓN (TORNEO BINARIO)
    padres=poblacion1.seleccion();


    //CRUCE UNIFORME + REPARADOR
    //Genera 1 hijo a partir de 2 padres. Para generar dos hijos, lo ejecutamos
    //2 veces a partir de los mismos padres.
    //siempre se cruzan los dos padres
    hijos = poblacion1.cruceUniforme(m, padres);


    //MUTACIÓN, PROB de mutar un gen es de 0.001
    poblacion1.OperadorMutacion(hijos);

    //EVALUAR LA POBLACIÓN
    coste1 = CosteEstimado(hijos.first, dist);
    coste2 = CosteEstimado(hijos.second, dist);
    eval+=2;


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
    AGEuniforme(m, distancias);
    double total = clock() - start_time;

    //cout << "Ejecutando: " << archivo <<endl;
    cout << endl << "Tiempo: " << total/CLOCKS_PER_SEC <<endl;

    entrada.close();
  }else
      cout << "No se puede abrir el fichero " << argv[1] <<endl;
}
