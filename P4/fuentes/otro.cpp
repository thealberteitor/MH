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
#include<numeric>
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



void BusquedaTabu(int m, vector<vector<double> > dist){
  const int EVALUACIONES = 100000;

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
    cerr << "COSTE nuevo: " << z << endl;
    eval++;

    for(int i=0; i<dist.size(); i++){
      if(sol[i]){
        quality[i]= (quality[i]*(freq[i]+z))/freq[i];
      }
    }



    sol.clear();
    total=0;
  }
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
    BusquedaTabu(m, distancias);
    double total = clock() - start_time;

    //cout << "Ejecutando: " << archivo <<endl;
    cout << endl << "Tiempo: " << total/CLOCKS_PER_SEC <<endl;

    entrada.close();
  }else
      cout << "No se puede abrir el fichero " << argv[1] <<endl;
}
