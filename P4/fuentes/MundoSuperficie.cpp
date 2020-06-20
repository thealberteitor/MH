#include <iostream>
#include <fstream>
#include <vector>
#include "time.h"
#include <utility> //pair
#include<algorithm>
#include<unistd.h>
#include <chrono>
#include <random>
#include "../inc/MundoSuperficie.h"
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
bool U(double prob, unsigned seed = rand()){
  static std::mt19937 gen{seed};
  std::uniform_real_distribution<> dist;
  return dist(gen) < prob;
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


















int s_center(vector<bool> all, vector<vector<double> > dist){
  vector<double> sumafilas(all.size());
  double suma=0;
  for(int i=0; i<all.size(); i++){

    for(int j=0; j<all.size(); j++){
      suma += dist[i][j];
    }

    sumafilas[i]=suma/sumafilas.size();
    suma=0;
  }

  double dev=0;
  for(int i=0; i<sumafilas.size(); i++)
    dev+=sumafilas[i];

  dev = dev/sumafilas.size();
  //return dev/sumafilas.size();
  //Buscamos el elemento más cercano al centro
  double min=9999999;
  int indice=0;

  for(int i=0;i<sumafilas.size(); i++){
    if(abs(sumafilas[i]-dev)<min){
      min=abs(sumafilas[i]-dev);
      indice=i;
    }
  }

  return indice;
}



double sel_center(vector<bool> all, vector<vector<double> > dist){
  int t = count(all.begin(), all.end(), true);
  vector<double> sumafilas(all.size());
  double suma;

  for(int i=0; i<all.size(); i++){
    if(all[i]){
      suma=0;
      for(int j=0; j<all.size(); j++){
        suma += dist[i][j];
      }
      sumafilas[i]=suma/sumafilas.size();
    }
    else
      sumafilas[i]=0;
  }


  double dev=0;
  for(int i=0; i<sumafilas.size(); i++)
    dev+=sumafilas[i];

  int d = count(sumafilas.begin(), sumafilas.end(), 0);
  dev = dev/(all.size()-d);


  //Buscamos el elemento más cercano al centro
  double min=9999999;
  int indice=0;

  for(int i=0;i<sumafilas.size(); i++){
    if(abs(sumafilas[i]-dev)<min){
      min=abs(sumafilas[i]-dev);
      indice=i;
    }
  }

  //cerr << abs(sumafilas[indice]-dev) << endl;
  return indice;
}



vector<bool> BusquedaTabu(int m, vector<vector<double> > dist){
  const int EVALUACIONES =1;

  int n = dist.size();
  vector<int> freq(n,0);
  vector<int> quality(n,0);
  vector<bool> sol(n,0);

  int eval=0, total=0;
  int centroide = s_center(sol, dist);
  double delta = 0.01, beta=0.01;

  double max_f, max_q, energy;
  double max_dist;
  double dist_p;
  //vector<double> dist_p(dist.size());
  int indice;
  double z;
  while(eval<EVALUACIONES){
    sol.clear();
    sol.resize(n,0);
    while(total<m){
      max_dist=0;
      for(int i=0; i<sol.size(); i++){
        if(!sol[i]){
          max_f = *max_element(freq.begin(), freq.end());
          max_q = *max_element(quality.begin(), quality.end());
          energy = dist[i][centroide];

          if(max_f == 0 && max_q==0)
            dist_p=energy;
          else if(max_f==0)
            dist_p = energy+ delta*energy*(quality[i]/max_q);
          else if(max_q==0)
            dist_p = energy-(beta*energy) * freq[i] / max_f;
          else
            dist_p = energy-(beta*energy) * freq[i] / max_f + delta*energy*(quality[i]/max_q);

          if(dist_p>max_dist){
            max_dist=dist_p;
            indice=i;
          }
        }
      }
      sol[indice]=true;
      total++;
      freq[indice]++;
      centroide = sel_center(sol, dist);

      //cerr << centroide << endl;
    }

    z = CosteEstimado(sol,dist);
    //cerr << "COSTE nuevo: " << z << endl;
    eval++;

    for(int i=0; i<dist.size(); i++){
      if(sol[i]){
        quality[i]= (quality[i]*(freq[i]+z))/freq[i];
      }
    }




    total=0;
  }
  return sol;
}




















//constructor, le pasamos la matriz de distancias
poblacion::poblacion(string nombre, int k, int j, vector<vector<double> > distances){
  n=15;
  id_k = k;
  id_j = j;
  superficie=nombre;
  soluciones={};
  values={};
  dist=distances;
  best=0;

  for(int i=0; i<n; i++)
    ones.push_back(0);

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



int poblacion::TorneoBinario(int sol1, int sol2){
  return (values[sol1]>values[sol2]) ? sol1 : sol2;
}


poblacion poblacion::seleccion(){
  poblacion poblacion2(" ", 0, 0, dist);

  int ganador;
  for(int i=0; i<n; i++){
    ganador = TorneoBinario(rand()%n, rand()%n);
    poblacion2.soluciones.push_back(soluciones[ganador]);
  }
  return poblacion2;
}






void poblacion::elitismo(vector<bool> mejor, double coste){
  int peor=distance(values.begin(), min_element(values.begin(), values.end()));
  soluciones[peor]=mejor;
  values[peor]=coste;

  if(values[peor]>values[best])
    best = peor;
}



void poblacion::movimiento(){

  if(superficie=="Botella de Klein"){
    int s ;

    for(int i=0; i<n; i++){
      int id=rand()%4;

      switch (id){
        case 0: //k
          do{
            s=rand()%dist.size();
          }while(s==id_k);
          swap(soluciones[i][id_k], soluciones[i][s]);

          break;

        case 1:  //j
        case 3:
          do{
            s=rand()%dist.size();
          }while(s==id_j);
          swap(soluciones[i][id_j], soluciones[i][s]);

          break;

        case 2: //n-k
          do{
            s=rand()%dist.size();
          }while(s==id_k);
          swap(soluciones[i][dist.size()-id_k], soluciones[i][s]);
          break;
      }
    }


  }else if(superficie=="Plano Proyectivo"){

    int s ;

    for(int i=0; i<n; i++){
      int id=rand()%4;

      switch (id) {
        case 0: //k
          do{
            s=rand()%dist.size();
          }while(s==id_k);
          swap(soluciones[i][id_k], soluciones[i][s]);

          break;

        case 1: //j
          do{
            s=rand()%dist.size();
          }while(s==id_j);
          swap(soluciones[i][id_j], soluciones[i][s]);
          break;

        case 2: //n-k
          do{
            s=rand()%dist.size();
          }while(s==id_k);
          swap(soluciones[i][dist.size()-id_k], soluciones[i][s]);
          break;

        case 3: //n-j
          do{
            s=rand()%dist.size();
          }while(s==id_j);
          swap(soluciones[i][dist.size()-id_j], soluciones[i][s]);
          break;
      }
    }

  }else if(superficie=="Toro Llano"){
    int s ;

    for(int i=0; i<n; i++){
      int id=rand()%4;

      switch (id) {
        case 0:
        case 2:
          do{
            s=rand()%dist.size();
          }while(s==id_k);
          swap(soluciones[i][id_k], soluciones[i][s]);

          break;

        case 1:
        case 3:
          do{
            s=rand()%dist.size();
          }while(s==id_j);
          swap(soluciones[i][id_j], soluciones[i][s]);
          break;
      }
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



void inicializarId(int &id_k, int &id_j, int n){
  id_k = rand()%n;
  do{
    id_j = rand()%n;
  }while(id_k==id_j);
}


vector<bool> poblacion::mutacionBrusca(vector<bool> sel){
  int n=sel.size();
  int total =  n/100;
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


void poblacion::nutrirse(){
  for(int i=0; i<n; i++){
    if(U(0.03))
      ones[i]++;
  }
}

void poblacion::mudar_piel(){
  int n_inicial = 15;
  vector<bool> res;
  for(int i=0; i<n; i++){
    if(ones[i]>=soluciones[i].size()/3){
      ones[i]=0;
      res = mutacionBrusca(soluciones[i]);
      if(n<=n_inicial*1.5){
        n++;
        soluciones.push_back(res);
        values.push_back(CosteEstimado(res,dist));
        ones.resize(n+1);
      }else{
        int peor=distance(values.begin(), min_element(values.begin(), values.end()));
        values[peor] = CosteEstimado(res,dist);
        soluciones[peor]=res;
      }

    }
  }

}


void poblacion::aplicar_BL(){
  int npeores=n/2;
  int peor;
  pair< vector<bool>, double > bl;
  for(int i=0; i<npeores; i++){
      peor = distance(values.begin(), min_element(values.begin(), values.end()));
      bl.first=soluciones[peor];
      bl.second=values[peor];
      bl = EvaluaVecinos(bl, dist);
      soluciones[peor]=bl.first;
      values[peor]=bl.second;
  }
}

void poblacion::viaje(poblacion otra){
  int best = distance(otra.values.begin(), max_element(otra.values.begin(), otra.values.end()));

  soluciones.push_back(otra.soluciones[best]);
  values.push_back(otra.values[best]);
  n++;
  ones.resize(n+1);

}


void MundoSuperficie(int m, vector<vector<double> > dist){
  //Mundo Superficie
  const int EVALUACIONES=300000;
  int n=dist.size();
  int eval=0;

  //INICIALIZAR LA POBLACIÓN
  int id_k, id_j;

  inicializarId(id_k, id_j, n);
  poblacion bk("Botella de Klein", id_k, id_j, dist);

  inicializarId(id_k, id_j, n);
  poblacion pp("Plano Proyectivo", id_k, id_j, dist);

  inicializarId(id_k, id_j, n);
  poblacion tl("Toro Llano", id_k, id_j, dist);


  for(int i=0; i<bk.n; i++){
    //bk.soluciones.push_back(SolucionInicial(n,m));
    //pp.soluciones.push_back(SolucionInicial(n,m));
    //tl.soluciones.push_back(SolucionInicial(n,m));
    bk.soluciones.push_back(BusquedaTabu(m,dist));
    pp.soluciones.push_back(BusquedaTabu(m, dist));
    tl.soluciones.push_back(BusquedaTabu(m,dist));
  }

  cerr << "next step " << endl;
  bk.evaluarPoblacion(eval);
  pp.evaluarPoblacion(eval);
  tl.evaluarPoblacion(eval);


  vector<bool> mejor;

  while(eval<EVALUACIONES){
    mejor = bk.soluciones[bk.best];
    bk.movimiento();
    bk.evaluarPoblacion(eval);
    bk.elitismo(mejor, CosteEstimado(mejor,dist));
    bk.nutrirse();
    bk.mudar_piel();


    mejor = pp.soluciones[pp.best];
    pp.evaluarPoblacion(eval);
    pp.movimiento();
    pp.elitismo(mejor, CosteEstimado(mejor,dist));
    pp.nutrirse();
    pp.mudar_piel();

    mejor = tl.soluciones[tl.best];
    tl.evaluarPoblacion(eval);
    tl.movimiento();
    tl.elitismo(mejor, CosteEstimado(mejor,dist));
    tl.nutrirse();
    tl.mudar_piel();

    cerr << eval << endl;





    if(U(0.001)){

      pp.aplicar_BL();
      tl.aplicar_BL();
      tl.aplicar_BL();

      int op = distance(pp.values.begin(), max_element(pp.values.begin(), pp.values.end()));
      cerr << "NOMBRE OP: " << pp.values[op] << "#####################" << endl;

      bk.viaje(pp);
      pp.viaje(tl);
      tl.viaje(bk);
    }
  }

  cout << "\nTL: ";
  tl.mejor_indice();

  cout << "\nBK: " ;
  bk.mejor_indice();

  cout << "\nPP: ";
  pp.mejor_indice();



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
    MundoSuperficie(m, distancias);
    double total = clock() - start_time;

    //cout << "Ejecutando: " << archivo <<endl;
    cout << endl << "Tiempo: " << total/CLOCKS_PER_SEC <<endl;

    entrada.close();
  }else
      cout << "No se puede abrir el fichero " << argv[1] <<endl;
}
