#include <iostream>
#include <fstream>
#include <vector>
#include "time.h"
#include<algorithm>
#include <float.h> //DBL max

using namespace std;



//Versión 2, mirar mejor función de abajo
int dist2Conj2(vector<int> &sel, vector<int> &unSel, vector<vector<double> > dist){
  //hay que tomar el max de la distancia entre los dos conjuntos

  //sel = seleccionados
  //unSel = no seleccionados
  double min_iter=DBL_MAX;
  int ind=-1;
  vector<double> minSi_sel;

  vector<int>::iterator it;
  vector<int>::iterator it2 ;

  for(it=unSel.begin() ; it!= unSel.end(); it++){ //hacía i=0 to unsel.size, MAL
    for(it2=sel.begin(); it2!=sel.end(); it2++){  //hacia j=0 to sel.size, MAL
      min_iter=min (dist[*it][*it2], min_iter);
    }
    minSi_sel.push_back(min_iter);
    min_iter=DBL_MAX;
  }
  //Elemento máximo
  vector<double>::iterator maximo = max_element(minSi_sel.begin(), minSi_sel.end());
  ind = distance(minSi_sel.begin(), maximo);

  return ind;
}



//Versión 1, más pro
int dist2Conj(vector<int> &sel, vector<int> &unSel, vector<vector<double> > &dist) {

  vector<int>::iterator it;
  vector<int>::iterator it2;
  //vector<double> sumas;

  double maximo = DBL_MIN;
  double suma;
  int indice=-1;

  for (it=unSel.begin(); it!=unSel.end(); it++) {
    for(it2=sel.begin(); it2!=sel.end(); it2++){
      suma += dist[*it][*it2];
    }

    if(suma>maximo){
      maximo=suma;
      indice=*it;
    }
    suma=0;


    //sumas.push_back(suma);
    //suma=0;
  }
  return indice;
    //std::vector<double>::iterator maximo1 = std::max_element(sumas.begin(), sumas.end());
    //indice = std::distance(sumas.begin(), maximo1);

    /*
    for(int i=0; i<sumas.size(); i++){
      maximo=max(sumas[i], maximo);
    }

    vector<double>::iterator buscador = find(sumas.begin(), sumas.end(), maximo);
    indice =  distance(sumas.begin(), buscador) <<endl;
    */

}


vector<double> DistanciaAcumulada(vector<vector<double> > dist){
  int n=dist.size();
  vector<double> acumulada(n);

  //recorremos toda la matriz en vez de solo la diagonal
  for(int i=0; i<n; i++){
    for(int j=0; j<n; j++){
      acumulada[i]+=dist[i][j];
    }
  }
  return acumulada;
}


double CosteEstimado(vector<int> entrada, vector<vector<double> > distancias){
  double zms=0;
  vector<int>::iterator it;
  vector<int>::iterator it2 ;

  //ver fórmula seminario2
  for(it=entrada.begin() ; it!= entrada.end()-1; it++){
    for(it2=it+1; it2!=entrada.end(); it2++){
      zms+= distancias[*it][*it2];
    }
  }
  return zms;
}




void Greedy(int m, vector<vector<double> > dist){
  //hay que buscar m en total.
  int n=dist.size();

  vector<int> sel; //m
  vector<int> unSel; //n-m
  vector<double> acum(n);
  vector<int>::iterator it;
  //Inicializamos no seleccionados
  for(int i=0; i<n; i++)
    unSel.push_back(i);

  //Calculamos distancia acumulada de cada elemento al resto
  acum=DistanciaAcumulada(dist);

  //Elemento máximo
  vector<double>::iterator maximo = max_element(acum.begin(), acum.end());
  int indice = distance(acum.begin(), maximo);

  sel.push_back(indice);
  unSel.erase(unSel.begin()+indice);

  /* Más rápido pero más lioso
  swap(unSel[indice], unSel.back());
  unSel.erase(unSel.begin()+indice);
  unSel.pop_back();
  */
  while((int)sel.size()<m){

    int range= dist2Conj(sel, unSel, dist);
    sel.push_back(range);

    it = find(unSel.begin(), unSel.end(), range);
    int update = distance(unSel.begin(), it);
    unSel.erase(unSel.begin()+update);
  }

  cout << "Greedy - coste estimado: " << CosteEstimado(sel, dist) <<endl;
}



int main(int argc, char *argv[]){
  //ifstream entrada("MDG-b_9_n500_m50.txt");
  string s, line, archivo = argv[1];
  ifstream entrada(archivo);

  int n,m;

  //matriz de distancias entre elementos
  //Almacenamos toda la matriz, necesaria para despues calcular
  //las distancias acumuladas
  int i,j;
  double d;

  if(entrada.is_open()){
    cout << "--------------------------------------"<<endl;
    cout << "Fichero " << argv[1] << " abierto correctamente." << endl;

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
    Greedy(m, distancias);
    double total = clock() - start_time;

    //cout << "Ejecutando: " << archivo <<endl;
    cout << "Tiempo: " << total/CLOCKS_PER_SEC <<endl;

    entrada.close();
  }else
      cout << "No se puede abrir el fichero " << argv[1] <<endl;
}
