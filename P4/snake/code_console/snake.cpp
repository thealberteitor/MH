/*
Para compilar (en linux):
	g++ snake.cpp -o snake -std=gnu++17
Para ejecutar:
	./snake
Puede funcionar aleatoriamente o introduciendo la direccion de la serpiente
Para girar, utilizar "d" y "a".
Para funcionar aleatoriamente, comentar en el main la funcion input y descomentar la funcion automatic
Para funcionar introduciendo la direccion, hacerlo a la inversa.
*/

#include <iostream>
#include <deque>
#include <algorithm>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

static const int NROW = 20;
static const int NCOL = 31;


class Snake{
	private:
		deque< pair<int,int> > positions;
		short direction; // 0 arriba 1 dcha 2 abajo 3 izda
	public:
		Snake(int init_x, int init_y, int init_len){	
			for(int i=0; i<init_len; i++){
				pair<int,int> tmp(init_x,init_y+i);
				positions.push_back(tmp);
			}
			direction=0;
		}
		
		deque< pair<int,int> > &getPos(){
			return positions;
		}
		
		short &getDirection(){
			return direction;
		}
		
		void setHead(pair<int,int> pos){
			positions.front()=pos;
		}
		
		void turnRight(){
			direction=(direction+1)%4;
		}
		
		void turnLeft(){
			direction=(direction+3)%4;
		}
		
		void move(){
			pair<int,int> tmp=positions.front();
			positions.pop_back();
			
			if(direction==0){
				tmp.second=(tmp.second-1+NROW)%NROW;
				positions.push_front(tmp);
			}
			
			if(direction==1){
				tmp.first=(tmp.first+1+NCOL)%NCOL;
				positions.push_front(tmp);
			}
			
			if(direction==2){
				tmp.second=(tmp.second+1+NROW)%NROW;
				positions.push_front(tmp);
			}
			
			if(direction==3){
				tmp.first=(tmp.first-1+NCOL)%NCOL;
				positions.push_front(tmp);
			}
		}
		
		void grow(){
			positions.push_back(positions.back());
		}
		
		bool isDead(){
			if(find(positions.begin()+1,positions.end(),positions.front())!=positions.end())
				return true;
			else
				return false;
		}

};

class Food{
	private:
		pair<int,int> pos;
		vector< pair<int,int> > no_validas;
	public:
		Food():pos{rand()%NCOL,rand()%NROW}{
			for(int i=NCOL/6; i<NCOL/6+4;i++){
				for(int j=NROW/2-2; j<NROW/2+2; j++){
					no_validas.push_back({i,j});
					}
			}
			
			for(int i=5*NCOL/6-3; i<5*NCOL/6+1;i++){
				for(int j=NROW/2-2; j<NROW/2+2; j++){
					no_validas.push_back({i,j});
				}
			}
			
		}
		
		pair<int,int> &getPos(){
			return pos;
		}
		
		void newPos(Snake s){
			do{
				pos = {rand()%NCOL,rand()%NROW};
			} while(find(s.getPos().begin(), s.getPos().end(), pos) != s.getPos().end()
					|| find(no_validas.begin(), no_validas.end(), pos) != no_validas.end());
		}

};


class Identification{
	private:
		vector< pair<int,int> > pos_ini;
		vector< pair<int,int> > pos_fin;
		bool horizontal;
		bool changes_direction;
	public:
		Identification(pair<int,int> ini, pair<int,int> fin, int len, bool hor, bool change){
			changes_direction=change;
			horizontal=hor;
			
			if(horizontal){
				for(int i=0; i<len; i++){
					pair<int,int> tmp(ini.first+i,ini.second);
					pos_ini.push_back(tmp);
					
					pair<int,int> tmp2(fin.first+i,fin.second);
					pos_fin.push_back(tmp2);
				}
			}else{
				for(int i=0; i<len; i++){
					pair<int,int> tmp(ini.first,ini.second+i);
					pos_ini.push_back(tmp);
					
					pair<int,int> tmp2(fin.first,fin.second+i);
					pos_fin.push_back(tmp2);
				}
			}
		}
		
		auto& getPos_ini(){
			return pos_ini;
		}
		
		auto& getPos_fin(){
			return pos_fin;
		}
		
		void travel(Snake &s){
			pair<int,int> head=s.getPos().front();
			int offset_x=0;
			int offset_y=0;
			
			if(horizontal){
				if(s.getDirection()==0)			offset_y=1;
				else if(s.getDirection()==2)	offset_y=-1;
			}else{
				if(s.getDirection()==1)			offset_x=1;
				else if(s.getDirection()==3)	offset_x=-1;
			}
			
			if((find(pos_ini.begin(), pos_ini.end(), head) != pos_ini.end())
			 && (
			   (horizontal && (s.getDirection()==0 || s.getDirection()==2))
			   ||
			   (!horizontal && (s.getDirection()==1 || s.getDirection()==3))
			 )
			){
				if(changes_direction){
					s.turnLeft();
					s.turnLeft();
					
					if(horizontal){	// Faltaria hacerlo para el caso vertical
						int indice=head.first-pos_ini.front().first;
						int correspondiente=(pos_ini.size()-1)-indice;
												
						offset_x+=(correspondiente-indice);
					}
				}
				
				s.setHead({head.first+pos_fin.front().first-pos_ini.front().first+offset_x,
				           head.second+pos_fin.front().second-pos_ini.front().second+offset_y});
				
			}
			if((find(pos_fin.begin(), pos_fin.end(), head) != pos_fin.end())
			 && (
			   (horizontal && (s.getDirection()==0 || s.getDirection()==2))
			   ||
			   (!horizontal && (s.getDirection()==1 || s.getDirection()==3))
			 )
			){
				if(changes_direction){
					s.turnLeft();
					s.turnLeft();
					
					if(horizontal){	// Faltaria hacerlo para el caso vertical
						int indice=head.first-pos_fin.front().first;
						int correspondiente = (pos_ini.size()-1)-indice;
						
						offset_x+=(correspondiente-indice);
					}
				}
				
				s.setHead({head.first+pos_ini.front().first-pos_fin.front().first+offset_x,
				           head.second+pos_ini.front().second-pos_fin.front().second+offset_y});
			}		
		}
};




void print(Snake s, Food f, vector<Identification> id){
	deque< pair<int,int> > pos_snake=s.getPos();
	pair<int,int> pos_food=f.getPos();
	
	for(int i=0; i<pos_snake.size(); i++){
		cout << "(" << pos_snake[i].first << "," << pos_snake[i].second << ");  ";
	}
	cout << endl;
	
	for(int i=0; i<NCOL; i++)	cout << "- ";
	cout << endl;
	
	bool ident=false;
	
	for(int i=0; i<NROW; i++){
		for(int j=0; j<NCOL; j++){
			pair<int,int> tmp(j,i);
			
			for(int k=0; k<id.size() && !ident; k++){
				if(find(id[k].getPos_ini().begin(), id[k].getPos_ini().end(), tmp) != id[k].getPos_ini().end()
				|| find(id[k].getPos_fin().begin(), id[k].getPos_fin().end(), tmp) != id[k].getPos_fin().end()){
					cout << "· ";
					ident=true;
				}
			}
						
			if(!ident){
				if(find(pos_snake.begin(), pos_snake.end(), tmp) != pos_snake.end()){
					cout << "# ";
				}else if(j==pos_food.first && i==pos_food.second){
					cout << "O ";
				}else{
					cout << "  ";
				}
			}
			ident=false;
		}
		cout << endl;
	}
	
	for(int i=0; i<NCOL; i++)	cout << "- ";
	cout << endl;
}

void input(Snake &s){
	char input;
	cin>>input;
	if(input=='a')	s.turnLeft();
	if(input=='d')	s.turnRight();

}

void automatic(Snake &s, int &i){
	i++;
	i=i%8;
	
	if(i==0){
		int nose=rand()%3;
		if(nose==0){
			s.turnRight();
		}else if(nose==1){
			s.turnLeft();
		}
	}
	
	//usleep(50000);
}





void update(Snake &s, Food &f, vector<Identification> id){
	s.move();
	
	for(int i=0;i<id.size();i++) id[i].travel(s);
	
	if(s.getPos().front()==f.getPos()){
		s.grow();
		f.newPos(s);
	}
}



int main(){
	srand(1);
	
	Snake s(NCOL/2, NROW/2, 5);
	Food f;
	int len=4;
	Identification id_1({NCOL/6,NROW/2-(len/2)},{5*NCOL/6,NROW/2-(len/2)},len,false,false);			// Vertical, no cambia
	Identification id_2({NCOL/6,NROW/2-(len/2)}, {5*NCOL/6-(len-1),NROW/2-(len/2)},len,true,true);			// Horizontal, cambia
	Identification id_3({NCOL/6+(len-1),NROW/2-(len/2)},{5*NCOL/6-(len-1),NROW/2-(len/2)},len,false,false);		// Vertical, no cambia
	Identification id_4({NCOL/6,NROW/2+(len-1)-(len/2)}, {5*NCOL/6-(len-1),NROW/2+(len-1)-(len/2)},len,true,true);		// Horizontal, cambia
	
	vector<Identification> id;
	id.push_back(id_1);
	id.push_back(id_2);
	id.push_back(id_3);
	id.push_back(id_4);
			
	int i=0;
	
	do{
		print(s,f,id);
		
		//input(s);
		
		automatic(s,i);
		
		update(s,f,id);
		
		usleep(50000);
	}while(!s.isDead());


	return 0;
}
