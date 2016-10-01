#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <queue>
using namespace std;
using namespace sf;

//Para la expansion
/*struct Estado{
    int x;
    int y;
};
queue<Estado> Cola;*/

const int MAX_ROWS = 12; //MAXIMO NUMERO DE FILAS Y COLUMNAS
bool Visitados[MAX_ROWS][MAX_ROWS];
bool Banderas[MAX_ROWS][MAX_ROWS];
int Tablero[MAX_ROWS][MAX_ROWS]; //TABLERO PARA FUNCION INTERNA
const int MAX_SQUARE = 32;

int dx[] = {-1,-1,-1,0,0,1,1,1}; //VECTORES DE DIRECCION
int dy[] = {-1,0,1,-1,1,-1,0,1}; //VECTORES DE DIRECCION
int destapadas = 0;

void initTablero(){ //VALORES POR DEFECTO DE LOS TRES TABLEROS
    for(int i=0;i<MAX_ROWS;i++){
        for(int j=0;j<MAX_ROWS;j++){
            Visitados[i][j] = false;
            Tablero[i][j] = 0;
            Banderas[i][j] = false;
        }
    }
}
void generarMinas(){ //GENERA MAX_ROWS MINAS ALEATORIAS
    int x,y;
    for(int i=0;i<MAX_ROWS;i++){
        x = rand()%MAX_ROWS;
        y = rand()%MAX_ROWS;
        while(Tablero[x][y] == -1){ //PREVIENE GENERAR UNA MINA EN UNA CASILLA YA UTILIZADA
            x = rand()%MAX_ROWS;
            y = rand()%MAX_ROWS;
        }
        //cout << x << " , " << y << endl;
        Tablero[x][y] = -1;
        for(int j=0;j<8;j++){
            if(x+dx[j] >= 0 && x+dx[j]<MAX_ROWS && y+dy[j]>=0 && y+dy[j]<MAX_ROWS){ //SETEAR NUMEROS ALREDEDOR DE CADA MINA PLANTADA
                if(Tablero[x+dx[j]][y+dy[j]] != -1) Tablero[x+dx[j]][y+dy[j]]++;
            }
        }
    }
}
/*void expandir(int x,int y){ //EXPANSION DE LAS CASILLAS VACIAS
    Estado aux;
    aux.x = x;
    aux.y = y;
    Cola.push(aux);
    Visitados[x][y] = true;
    destapadas++;
    while(!Cola.empty()){
        Estado actual = Cola.front();
        Cola.pop();
        if(Tablero[actual.x][actual.y] == 0){ //SIGUE EXPANDIENDO CUANDO SE ENCUENTRA UNA CASILLA VACÍA
            for(int i=0;i<8;i++){
                int adX = actual.x+dx[i];
                int adY = actual.y+dy[i];
                if(adX >= 0 && adX < MAX_ROWS && adY >=0 && adY < MAX_ROWS){ //VALIDA POSICION
                    if(Tablero[adX][adY] != -1 && !Visitados[adX][adY]){ //ADYACENTE NO ES MINA Y NO HA SIDO EXPLORADO
                        Estado encolado;
                        encolado.x = adX;
                        encolado.y = adY;
                        Cola.push(encolado);
                        Visitados[adX][adY] = true;
                        destapadas++;
                    }
                }
            }
        }
    }
}*/
void expandirDFS(int x,int y){
    int adX,adY;
    Visitados[x][y] = true;
    for(int i=0;i<8;i++){
        adX = x+dx[i];
        adY = y+dy[i];
        if(adX >= 0 && adX < MAX_ROWS && adY >=0 && adY < MAX_ROWS){
            if(Tablero[adX][adY] ==0 ){
                destapadas++;
                expandir(adX,adY);
            }
            else{
                Visitados[adX][adY] = true;
            }
        }
    }
    destapadas--;
}
void visitarTodo(){
    for(int i=0;i<MAX_ROWS;i++){
        for(int j=0;j<MAX_ROWS;j++){
            Visitados[i][j] = true;
            Banderas[i][j] = false;
        }
    }
}
//PARA DEBUG
char conviertePosicion(int a){
    string nums = "012345678";
    if(a == 0) return '.'; //Espacio vacio
    if(a == -1) return '*'; //Mina
    else return nums[a]; //Numero, pero a caracter
}
//PARA DEBUG
void printTablero(){
    for(int i=0;i<MAX_ROWS;i++){
        for(int j=0;j<MAX_ROWS;j++){
            cout << conviertePosicion(Tablero[i][j]) << " ";
        }
        cout << endl;
    }
}
int main(){
    srand(time(0));
    initTablero();
    generarMinas();
    //printTablero();

    RenderWindow window(VideoMode(385,385), "The Mineswepper", Style::Titlebar | Style::Close);

    Texture t;
    t.loadFromFile("images/tiles4.jpg");
    Sprite s(t);
    while(window.isOpen()){
        Vector2i coord = Mouse::getPosition(window);
        int x = coord.x/MAX_SQUARE;
        int y = coord.y/MAX_SQUARE;
        Event evento;
        while(window.pollEvent(evento)){
            if(evento.type == Event::Closed){
                window.close();
            }
            if(evento.type == Event::MouseButtonPressed){
                if(evento.key.code == Mouse::Left){
                    if(!Banderas[y][x]){ //Si hay una bandera no se puede presionar el boton
                        if(Tablero[y][x] == 0 && !Visitados[y][x]){ //Si casilla vacia expandir
                            expandirDFS(y,x);
                        }
                        else if(Tablero[y][x] == -1 ){ //Mina termina el juego
                            visitarTodo();
                            //sonidoExplosion.play();
                        }
                        else{
                            if(!Visitados[y][x]){
                               Visitados[y][x] = true;
                               destapadas++; //hay numerito abajo, no hay expansion
                            }
                        }
                    }
                    if(destapadas==(MAX_ROWS*MAX_ROWS - MAX_ROWS)){
                        visitarTodo();
                        //sonidoGano.play();
                    }
                }
                if(evento.key.code == Mouse::Right){ // Poner/Quitar bandera
                    if(!Visitados[y][x]) Banderas[y][x] = !Banderas[y][x];
                }
            }
        }

        //Para imprimir tablero
        window.clear();
        int act;
        for(int i=0;i<MAX_ROWS;i++){
            for(int j=0;j<MAX_ROWS;j++){
                //if(Tablero[x][y] == -1) visitarTodo();
                if(Visitados[i][j]){
                    if(Tablero[i][j] == -1) act = 9;
                    else act = Tablero[i][j];
                    /*else if(Tablero[i][j] != 0) act = Tablero[i][j];
                    else act = 0;*/
                }
                else act = 10;
                if(Banderas[i][j]) act = 11;
                s.setTextureRect(IntRect(act*MAX_SQUARE,0,MAX_SQUARE,MAX_SQUARE));
                s.setPosition(j*MAX_SQUARE,i*MAX_SQUARE);
                window.draw(s);
            }
        }
        window.display();
    }
    return 0;
}
