#include <iostream>
#include <SDL.h>
#include <ctime>
#include <vector>
#include <SDL_image.h>
using namespace std;

const int SCREEN_HEIGHT = 600;
const int SCREEN_WIDTH = 600;
const int scale = 20;


/*struct Snake{
    SDL_Rect sn;
    vector<int> tailX;
    vector<int> taily;
    int tailLength;
    Snake(){
        sn.x=0;
        sn.y=0;
        sn.w=scale;
        sn.h=scale;
        tailLength = 0;
    }

    void render(){

    }
};*/

void randomFoodPos (SDL_Rect &food, const SDL_Rect &player);

void initSDL(SDL_Window* &window, SDL_Renderer* &renderer);

void waitUntilKeyPressed();

void quitSDL(SDL_Window* window, SDL_Renderer* renderer);

void moving(SDL_Rect &player, const int &stepX, const int &stepY){
    player.x += stepX;
    player.y += stepY;
}

void turnLeft( int &stepX, int &stepY);
void turnRight(int &stepX, int &stepY);
void turnUp(int &stepX, int &stepY);
void turnDown(int &stepX, int &stepY);

SDL_Surface* loadSurface( string path, SDL_Surface* gScreenSurface );

void gameoverDis (SDL_Window* &window, SDL_Surface* screenSurface);
void youWinDis (SDL_Window* &window, SDL_Surface* screenSurface);

void getEve

int main (int argc, char* argv[]){
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Surface* screenSurface;
	SDL_Event e;

    initSDL(window, renderer);
    screenSurface = SDL_GetWindowSurface(window);

    SDL_Rect player;
    player.x = 20;
    player.y = 0;
    player.w = scale;
    player.h = scale;
    int stepX=0, stepY=0;

    SDL_Rect tail;
    tail.w = scale;
    tail.h = scale;
    int tailLength=0;
    vector<int> tailx;
    vector<int> taily;

    SDL_Rect food;
    food.x = rand()*scale %SCREEN_WIDTH;
    food.y = rand()*scale %SCREEN_HEIGHT;
    food.w = scale;
    food.h = scale;

    int DelayTime = 80;

    bool gameOver = 0, youWin=0;
    while (!gameOver){
        {//Draw
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);

            SDL_SetRenderDrawColor(renderer, 255, 200, 0, 0);
            SDL_RenderFillRect(renderer, &food);

            SDL_SetRenderDrawColor(renderer, 18, 200, 61, 255);
            SDL_RenderFillRect(renderer, &player);

            for (int i=0; i<tailLength; i++){
                tail.x = tailx[i];
                tail.y = taily[i];
                SDL_RenderFillRect(renderer, &tail);
            }

            SDL_RenderPresent(renderer);
        }

        //Logic
            if (player.x == food.x && player.y == food.y) {
                    randomFoodPos (food, player);
                    tailx.push_back(player.x);
                    taily.push_back(player.y);
                    tailLength++;
                    if (DelayTime >10) DelayTime --;
            }

            if (player.x+scale>SCREEN_WIDTH || player.y+scale>SCREEN_HEIGHT
                ||player.x<0 || player.y<0) gameOver=1;
            for (int i=0; i<=tailLength-4; i++){
                if (player.x == tailx[i] && player.y == taily[i]) gameOver = 1;
            }
            if (tailLength>=600) youWin=1;

            SDL_Delay(DelayTime);
            for (int i=0; i<tailLength-1; i++){
                        tailx[i] = tailx[i+1];
                        taily[i] = taily[i+1];
                    }
                if(tailLength>0){
                    tailx[tailLength-1] = player.x;
                    taily[tailLength-1] = player.y;
                }

            moving(player, stepX, stepY);

            if ( SDL_PollEvent(&e) == 0) continue;

            if (e.type == SDL_QUIT) break;

            if (e.type == SDL_KEYDOWN){
                switch (e.key.keysym.sym) {

                    case SDLK_ESCAPE: break;

                    case SDLK_LEFT: if(stepX<=0) turnLeft(stepX, stepY) ;
                            break;

                    case SDLK_RIGHT: if(stepX!=-scale)turnRight(stepX, stepY);
                            break;
                    case SDLK_DOWN: if(stepY!=-scale)turnDown(stepX, stepY);
                            break;
                    case SDLK_UP: if(stepY!=scale) turnUp(stepX, stepY);
                            break;
                    default: break;
                }

            }

        }
    if (gameOver) gameoverDis(window, screenSurface);
    if (youWin) youWinDis (window, screenSurface);

    waitUntilKeyPressed();
    quitSDL(window, renderer);
    return 0;
}

void initSDL(SDL_Window* &window, SDL_Renderer* &renderer){
    window = SDL_CreateWindow("Snake game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) cout << "fail CreateWindow: "<<SDL_GetError()<<endl;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED |SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) cout << "fail CreateRenderer: "<<SDL_GetError()<<endl;

}

void randomFoodPos (SDL_Rect &food, const SDL_Rect &player){
    while (player.x == food.x && player.y == food.y){
                food.x = rand()*scale%SCREEN_WIDTH;
                food.y = rand()*scale%SCREEN_HEIGHT;
            }
}

void waitUntilKeyPressed()
{
    SDL_Event e;
    while (true) {
        if ( SDL_WaitEvent(&e) != 0 &&
             (e.type == SDL_KEYDOWN || e.type == SDL_QUIT) )
            return;
        SDL_Delay(100);
    }
}

SDL_Surface* loadSurface( string path, SDL_Surface* gScreenSurface )
{
    SDL_Surface* optimizedSurface = NULL;
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL )
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
    }
    else
    {
        optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, 0 );
        if( optimizedSurface == NULL )
        {
            printf( "Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
        }
        SDL_FreeSurface( loadedSurface );
    }
    return optimizedSurface;
}

void gameoverDis (SDL_Window* &window, SDL_Surface* screenSurface){
    SDL_Surface* image;
    image = loadSurface("GameOver.jpg", screenSurface);
        SDL_Rect a;
        a.x = 200;
        a.y = 200;
        a.w = 200;
        a.h = 200;
        SDL_BlitScaled(image, NULL, screenSurface, &a);

        SDL_UpdateWindowSurface(window);
}

void youWinDis (SDL_Window* &window, SDL_Surface* screenSurface){
    SDL_Surface* image;
    image = loadSurface("YouWin.jpg", screenSurface);
        SDL_Rect a;
        a.x = 200;
        a.y = 200;
        a.w = 200;
        a.h = 200;
        SDL_BlitScaled(image, NULL, screenSurface, &a);

        SDL_UpdateWindowSurface(window);
}

void turnLeft( int &stepX, int &stepY){
    stepX = -scale;
    stepY = 0;
}
void turnRight(int &stepX, int &stepY){
    stepX = scale;
    stepY = 0;
}
void turnUp(int &stepX, int &stepY){
    stepX = 0;
    stepY = -scale;
}
void turnDown(int &stepX, int &stepY){
    stepX = 0;
    stepY = scale;
}

void quitSDL(SDL_Window* window, SDL_Renderer* renderer){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow (window);
    SDL_Quit();
}
