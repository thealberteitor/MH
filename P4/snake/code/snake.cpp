// El comando para compilar es, con gcc >= 7:
//
// $> g++ -O2 -std=c++17 snake.cpp -o snake -lsfml-graphics -lsfml-window -lsfml-system
//
// Hace falta instalar SFML:
//
// sudo apt-get install libsfml-dev

#include <algorithm>
#include <deque>
#include <utility>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

static const int NCOL = 79;
static const int NROW = 49;


static const sf::Color BACK(255, 255, 250);
static const sf::Color FOOD(255, 30, 30);
static const sf::Color IDENT[4]={sf::Color(148, 37, 230),sf::Color(0, 37, 230),sf::Color(148, 37, 0),sf::Color(148, 255, 230)};
static const sf::Color AGUJERO(64,64,64);

class Snake
{
	private:
		std::deque<std::pair<int,int>> positions;
		short direction; // 0 arriba 1 dcha 2 abajo 3 izda
	public:
		Snake(int init_x, int init_y, int init_len){
			for(int i=0; i<init_len; i++)
				positions.emplace_back(init_x, init_y+i);

			direction=0;
		}

		auto& getPos(){
			return positions;
		}
		
		short &getDirection(){
			return direction;
		}
		
		void setHead(std::pair<int,int> pos){
			positions.front()=pos;
		}

		void turnRight(){
			direction = (direction+1)%4;
		}

		void turnLeft(){
			direction = (direction+3)%4;
		}

		void move(){
			std::pair<int,int> tmp = positions.front();
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
		
		void reset(){
			positions.clear();
			int init_x=NCOL/2;
			int init_y=NROW/2;
			int init_len=6;
			for(int i=0; i<init_len; i++)
				positions.emplace_back(init_x, init_y+i);

			direction=0;
		}

		bool isDead(){
			return find(positions.begin()+1,positions.end(),positions.front())!=positions.end();
		}

};

class Food{
	private:
		std::pair<int,int> pos;
		std::vector< std::pair<int,int> > no_validas;
	public:
		Food() : pos{rand()%NCOL,rand()%NROW}{
			for(int i=NCOL/6; i<NCOL/6+10;i++){
				for(int j=NROW/2-5; j<NROW/2+5; j++){
					no_validas.push_back({i,j});
					}
			}
			
			for(int i=5*NCOL/6-9; i<5*NCOL/6+1;i++){
				for(int j=NROW/2-5; j<NROW/2+5; j++){
					no_validas.push_back({i,j});
				}
			}
		
		}

		std::pair<int,int> &getPos(){
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
		std::vector< std::pair<int,int> > pos_ini;
		std::vector< std::pair<int,int> > pos_fin;
		bool horizontal;
		bool changes_direction;
		int longitud;
	public:
		Identification(std::pair<int,int> ini, std::pair<int,int> fin, int len, bool hor, bool change){
			changes_direction=change;
			horizontal=hor;
			longitud=len;
			
			if(horizontal){
				for(int i=0; i<len; i++){
					std::pair<int,int> tmp(ini.first+i,ini.second);
					pos_ini.push_back(tmp);
					
					std::pair<int,int> tmp2(fin.first+i,fin.second);
					pos_fin.push_back(tmp2);
				}
			}else{
				for(int i=0; i<len; i++){
					std::pair<int,int> tmp(ini.first,ini.second+i);
					pos_ini.push_back(tmp);
					
					std::pair<int,int> tmp2(fin.first,fin.second+i);
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
		
		int& getLen(){
			return longitud;
		}
		
		void travel(Snake &s){
			std::pair<int,int> head=s.getPos().front();
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



void update(Snake &s, Food &f, std::vector<Identification> id){
	s.move();
		
	for(int i=0;i<id.size();i++) id[i].travel(s);
	
	if(s.getPos().front()==f.getPos()){
		s.grow();
		f.newPos(s);
	}
}




class DrawableSnake : public sf::Drawable
{
public:
    DrawableSnake(Snake* snake_ptr, Food* food_ptr, std::vector<Identification>* id_ptr)
        : m_snake_ptr{snake_ptr}, m_food_ptr{food_ptr}, m_id_ptr{id_ptr}
    {}
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        static sf::RectangleShape square
        {
            sf::Vector2f
            {
                static_cast<float>(target.getView().getSize().x / NCOL),
                static_cast<float>(target.getView().getSize().y / NROW)
            }
        };
        square.setFillColor(sf::Color::Green);
        square.setOutlineColor(sf::Color{200, 200, 200});
        square.setOutlineThickness(1);

        for (auto [x, y] : m_snake_ptr->getPos())
        {
            square.setPosition(x * square.getSize().x, y * square.getSize().y);
            target.draw(square, states);
        }

        square.setFillColor(FOOD);
        square.setPosition
        (
            m_food_ptr->getPos().first * square.getSize().x,
            m_food_ptr->getPos().second * square.getSize().y
        );

        target.draw(square, states);
        
        
        for(int i=0; i<m_id_ptr->size(); i++){
        	square.setFillColor(IDENT[i]);
        	for (auto [x, y] : m_id_ptr->at(i).getPos_ini()){
            	square.setPosition(x * square.getSize().x, y * square.getSize().y);
            	target.draw(square, states);
        	}
        	for (auto [x, y] : m_id_ptr->at(i).getPos_fin()){
            	square.setPosition(x * square.getSize().x, y * square.getSize().y);
            	target.draw(square, states);
        	}
        }
        target.draw(square, states);
        
        
        // Bordes identificados
        // Horizontales
        sf::RectangleShape line(sf::Vector2f(target.getView().getSize().x, 5));
        line.setFillColor(sf::Color{255,0,0});
        target.draw(line, states);
        line.setPosition(0,target.getView().getSize().y-5);
        target.draw(line, states);
        
        // Verticales
        line.rotate(-90);
        line.setFillColor(sf::Color{0,0,255});
        target.draw(line,states);
        line.setPosition(target.getView().getSize().x-5,target.getView().getSize().y-5);
        target.draw(line,states);
        
        // Agujeros
        std::pair<int,int> pos_agujero_ini=m_id_ptr->front().getPos_ini().front();
        std::pair<int,int> pos_agujero_fin=m_id_ptr->front().getPos_fin().front();
        
        sf::RectangleShape agujero(sf::Vector2f(
        			(m_id_ptr->front().getLen()-2)*square.getSize().x,
        			(m_id_ptr->front().getLen()-2)*square.getSize().y )
        );
        
        agujero.setFillColor(AGUJERO);
        
        agujero.setPosition((pos_agujero_ini.first+1) * square.getSize().x,
        					(pos_agujero_ini.second+1) * square.getSize().y);
        target.draw(agujero,states);
        
        agujero.setPosition((pos_agujero_fin.first-m_id_ptr->front().getLen()+2) * square.getSize().x,
        					(pos_agujero_fin.second+1) * square.getSize().y);
        target.draw(agujero,states);
    }

    Snake* m_snake_ptr = nullptr;
    Food* m_food_ptr = nullptr;
    std::vector<Identification>* m_id_ptr=nullptr;
};

void increaseSpeed(sf::Time &period)
{
    if (period.asMilliseconds() > 1)
        period -= sf::milliseconds(1);
}

void decreaseSpeed(sf::Time &period)
{
    if (period.asMilliseconds() < 500)
        period += sf::milliseconds(1);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(NCOL*10, NROW*10), "Snake (not running)");

    Snake snake(NCOL/2, NROW/2, 6);
    Food food;
    int len=10;
	Identification id_1({NCOL/6,NROW/2-(len/2)},{5*NCOL/6,NROW/2-(len/2)},len,false,false);			// Vertical, no cambia
	Identification id_2({NCOL/6,NROW/2-(len/2)}, {5*NCOL/6-(len-1),NROW/2-(len/2)},len,true,true);			// Horizontal, cambia
	Identification id_3({NCOL/6+(len-1),NROW/2-(len/2)},{5*NCOL/6-(len-1),NROW/2-(len/2)},len,false,false);		// Vertical, no cambia
	Identification id_4({NCOL/6,NROW/2+(len-1)-(len/2)}, {5*NCOL/6-(len-1),NROW/2+(len-1)-(len/2)},len,true,true);		// Horizontal, cambia
	
	std::vector<Identification> id;
	id.push_back(id_1);
	id.push_back(id_2);
	id.push_back(id_3);
	id.push_back(id_4);
    
    
    
    
    DrawableSnake grid(&snake, &food, &id);

    bool running = false;
    sf::Clock timer;
    sf::Time period = sf::milliseconds(70);

    while (window.isOpen())
    {
        sf::Event event{};

        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyPressed:
                    switch (event.key.code)
                    {
                        case sf::Keyboard::Up:
                            increaseSpeed(period);
                            break;
                        case sf::Keyboard::Down:
                            decreaseSpeed(period);
                            break;
                        default:
                            break;
                    }
                    break;
                case sf::Event::KeyReleased:
                    switch (event.key.code)
                    {
                        case sf::Keyboard::Q:
                        case sf::Keyboard::Escape:
                            window.close();
                            break;
                        case sf::Keyboard::Space:
                            running = !running;

                            if (running)
                                window.setTitle("Snake (running)");
                            else
                                window.setTitle("Snake (not running)");

                            break;
                        case sf::Keyboard::A:
                            snake.turnLeft();
                            break;
                        case sf::Keyboard::D:
                            snake.turnRight();
                            break;
                        default:
                            break;
                    }

                    break;
                default:
                    break;
            }
        }

        if (running && timer.getElapsedTime() > period)
        {
            update(snake, food,id);
            timer.restart();
        }
        
        if(snake.isDead()){
    		snake.reset();
    		running=false;
    		window.setTitle("Snake (not running)");
    	}
    	
        window.clear(BACK);
        window.draw(grid);
        window.display();
    }
}
