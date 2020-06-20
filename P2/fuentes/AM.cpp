#include <iostream>
#include <fstream>
#include <vector>
#include "time.h"
#include <utility> //pair
#include<algorithm>
#include<unistd.h>
#include <chrono>
#include <random>
#include "../inc/AM.h"
#include<bits/stdc++.h>
#include<unistd.h>
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


//////////////////////////////////////////////////////////
//BUSQUEDA LOCAL


//tenemos las contribuciones independientes, tomar la que tenga menos contribución
double ContribucionIndepBL(int indice, vector<int> &sel, vector<vector<double> > &dist){
  vector<int>::iterator it;
  double sum=0;

  for(it=sel.begin(); it!=sel.end(); it++)
    sum += dist[indice][*it];

  return sum; //mirar seminario2, pag 24
}



//de la forma anterior (greedy no se puede hacer)
//Ya no se puede hacer así, ya que necesitamos calcular las contribuciones
//de forma independiente y así seleccionar los elementos que menos contribuyen
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



pair< vector<int>, double > EvaluaVecinos(vector<int> ini, double seco, vector<vector<double> > &dist, int &evalT){

  bool mejora = true, salir;
  int n = dist.size(), c, ind;
  int m = ini.size();
  int eval=0;

  const int MAX = 400;
  pair< vector<int>, double > inicial(ini, seco);

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
      return inicial;
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
      if( nueva>antigua){
        inicial.first[ind]=j;  //intercambio i por j
        inicial.second = inicial.second + nueva - antigua;; //actualizamos coste total (factorizado)

        salir=true;
        todasJ.clear();
        contribuciones.clear();
      }
      c++;
      eval++; //evaluaciones de la Búsqueda Local
      evalT++; //Evaluaciones del algoritmo memético
    }
  }
  return inicial;
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


/////////// CLASS POBLACION, ALGORITMO GENÉTICO /////////////////
//constructor, le pasamos la matriz de distancias
poblacion::poblacion(vector<vector<double> > distances){
  n=10;
  mut = 0.001;
  prob_cruce=0.7;
  tipo=3;
  soluciones={};
  values={};
  dist=distances;
  best=0;
  prob_ls=0.1;
}

/*
poblacion& poblacion::operator=(const poblacion &orig){
  if(this!=&orig){
    n=orig.n;
    mut=orig.mut;
    prob_cruce=orig.prob_cruce;
    tipo=orig.tipo;
    soluciones=orig.soluciones;
    values=orig.values;
    dist=orig.dist;
    best=orig.best;
    prob_ls=orig.prob_ls;
  }

  return *this;
}*/


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



vector< vector<int> > poblacion::pasarAEnteros(){
  vector<int> sel_i;
  vector< vector<int> > entera;

  for(int i=0; i<n; i++){
    sel_i.clear();
    for(int j=0; j<soluciones[i].size(); j++){
      if(soluciones[i][j]==true)
        sel_i.push_back(j);
    }
    entera.push_back(sel_i);
  }
  return entera;
}



void poblacion::pasarABinarios(vector< vector<int> > entera){
  for(int i=0; i<soluciones.size(); i++){
    for(int j=0; j<soluciones[i].size(); j++){
      soluciones[i][j]=false;
    }
  }

  for(int i=0; i<entera.size();i++){
    for(int j=0; j<entera[i].size(); j++){
      soluciones[i][entera[i][j]]=true;
    }
  }
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



void poblacion::memetico(int &eval){

  //Pasamos la población1 a codificación de conjuntos enteros.
  vector<vector<int> > entera = pasarAEnteros();
  int total=0, r1=0;
  pair< vector<int>, double > cadaUno;
  int nmejores, npeores, peor;


  switch(tipo){
    //TIPO 1
    //aplicar BL sobre todos los cromosomas de la población
    case 1:{
      for(int i=0; i<entera.size(); i++){
        cadaUno = EvaluaVecinos(entera[i], values[i], dist, eval);
        entera[i]=cadaUno.first;
        values[i] = cadaUno.second;
      }
      break;
    }
    //TIPO 2
    //aplicar BL sobre (prob_ls * n) cromosomas de la población
    case 2:{
      total = prob_ls * entera.size();
      while(total>0){
        r1 = rand()%entera.size();
        cadaUno=EvaluaVecinos(entera[r1], values[r1], dist, eval);
        entera[r1]=cadaUno.first;
        values[r1]=cadaUno.second;

        total--;
      }
      break;
    }
    case 3:{

      //TIPO 3
      //aplicar BL sobre los (prob_ls * n) mejores cromosomas de la población actual
      //Usamos la esperanza matemática
      nmejores=prob_ls * n;
      pair<int,double> otro;

      vector<pair<int, double> > estos(nmejores);
      for(int i=0; i<nmejores; i++){
          best = distance(values.begin(), max_element(values.begin(), values.end()));
          otro.first=best;
          otro.second=values[best];
          estos.push_back(otro);
          values[best]=0;
      }

      //tengo que hacerlo en otro bucle para no machacar los valores ni tomar dos veces el mismo cromosoma.
      for(int i=0; i<nmejores; i++){
        cadaUno=EvaluaVecinos(entera[estos[i].first], values[estos[i].second], dist, eval);
        entera[estos[i].first]=cadaUno.first;
        values[estos[i].first]=cadaUno.second;
      }

      break;
    }






    case 4:{  //AM-GREEDY1
      //TIPO 4
      //aplicar BL sobre el mejor cromosoma de la población que obtenemos en la población
      //actual un total de (prob_ls * n) veces.
      //Usamos la esperanza matemática
      nmejores=prob_ls * n;
      while(nmejores>0){

        cadaUno=EvaluaVecinos(entera[best], values[best], dist, eval);
        entera[best]=cadaUno.first;
        values[best]=cadaUno.second;
        best = distance(values.begin(), max_element(values.begin(), values.end()));

        nmejores--;
      }
      break;
    }
    case 5:{ //AM-GREEDY2
      //TIPO 5
      //aplicar BL sobre el peor cromosoma de la población que obtenemos en la población
      //un total de (prob_ls * n) veces.
      //Usamos la esperanza matemática
      npeores=prob_ls * n;
      while(npeores>0){

        peor = distance(values.begin(), min_element(values.begin(), values.end()));

        cadaUno=EvaluaVecinos(entera[peor], values[peor], dist, eval);
        entera[peor]=cadaUno.first;
        values[peor]=cadaUno.second;
        npeores--;
      }
      best = distance(values.begin(), max_element(values.begin(), values.end()));

      break;
    }

    case 6:{ //AM-WORST

      //TIPO 6
      //aplicar BL sobre los n/2 peores cromosomas de la población actual
    
      npeores=n/2;
      pair<int,double> otro;

      vector<pair<int, double> > estos(npeores);
      for(int i=0; i<npeores; i++){
          peor = distance(values.begin(), min_element(values.begin(), values.end()));
          otro.first=peor;
          otro.second=values[peor];
          estos.push_back(otro);
          values[peor]=0;
      }

      //tengo que hacerlo en otro bucle para no machacar los valores ni tomar dos veces el mismo cromosoma.
      for(int i=0; i<npeores; i++){
        cadaUno=EvaluaVecinos(entera[estos[i].first], values[estos[i].second], dist, eval);
        entera[estos[i].first]=cadaUno.first;
        values[estos[i].first]=cadaUno.second;
      }

      break;
    }



    default:{
      cout <<"Error, el tipo debe ser [1,6]";
      break;
    }
  }

  pasarABinarios(entera);
  evaluarPoblacion(eval);
}


void Alg_Memetico(int m, vector<vector<double> > dist){

  const int EVALUACIONES=100000;
  const int MAX_GEN=600;
  int n=dist.size();
  int eval=0, g=0;

  //INICIALIZAR LA POBLACIÓN
  poblacion poblacion1(dist);
  for(int i=0; i<poblacion1.n; i++)
    poblacion1.soluciones.push_back(SolucionInicial(n,m));

  //EVALUAR LA POBLACIÓN
  poblacion1.evaluarPoblacion(eval);

  poblacion poblacion2(dist);
  //while(g<MAX_GEN){ //para el ejercicio extra

  while(eval<EVALUACIONES){

    //OPERADOR DE SELECCIÓN (TORNEO BINARIO) tantos torneos como elementos en población
    poblacion2 = poblacion1.seleccion();


    //CRUCE  POSICIONAL
    //poblacion2.cruceUniforme(m);
    poblacion2.crucePosicional();

    //MUTACIÓN, PROB de mutar un gen es de 0.001
    poblacion2.mutacion();

    poblacion2.evaluarPoblacion(eval);


    /*
    if(g>10){
      poblacion2.tipo=1; //4 mejor, 5 peor
    }*/


    g++;
    if(g%10==0){
      poblacion2.memetico(eval);
    }


    //ESQUEMA DE REEMPLAZAMIENTO:
    //Para conservar el elitismo: si la mejor solución de la generación anterior
    //no sobrevive, sustituye directamente la peor sol de la nueva población.
    poblacion1.reemplazamiento(poblacion2);



    //cout << "EVAL: "   << eval <<endl;
    //cout << "MEJOR: " << poblacion1.values[poblacion1.best]<<endl;
  }
  //cout << "MEJOR: " << poblacion1.values[poblacion1.best]<<endl;
  cout << "MEJOR " << CosteEstimado(poblacion1.soluciones[poblacion1.best], dist)<<endl ;
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
    Alg_Memetico(m, distancias);
    double total = clock() - start_time;

    //cout << "Ejecutando: " << archivo <<endl;
    cout << endl << "Tiempo: " << total/CLOCKS_PER_SEC <<endl;

    entrada.close();
  }else
      cout << "No se puede abrir el fichero " << argv[1] <<endl;
}
