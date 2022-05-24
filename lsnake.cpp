#include <iostream>
#include <vector>

#include <ctime>
#include <SDL.h>
#include <SDL_image.h>
using namespace std;

void renderPlayer(SDL_Renderer* renderer, SDL_Rect player, int x, int y, int scale, vector<int> tailX, vector<int> tailY, int tailLength) {
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	player.w = scale;
	player.h = scale;

	// Gets x and y of all tail blocks and renders them
	for (int i = 0; i < tailLength; i++) {
		player.x = tailX[i];
		player.y = tailY[i];
		SDL_RenderFillRect(renderer, &player);
	}

	player.x = x;
	player.y = y;

	SDL_RenderFillRect(renderer, &player);
}

void renderFood(SDL_Renderer* renderer, SDL_Rect food) {
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 0);
	SDL_RenderFillRect(renderer, &food);
}

bool checkCollision(int foodx, int foody, int playerx, int playery) {
	if (playerx == foodx && playery == foody){
		return true;
	}
	return false;
}

// Get a valid spawn for the food which is not on top of a tail or player block
pair<int, int> getFoodSpawn(vector<int> tailX, vector<int> tailY, int playerX, int playerY, int scale, int wScale, int tailLength) {
	bool valid = true;
	int x = 0;
	int y = 0;
	srand(time(0));
	x = scale * (rand() % scale);
	y = scale * (rand() % scale);
	// Check all tail blocks and player block
	for (int i = 0; i < tailLength; i++) {
		if ((x == tailX[i] && y == tailY[i]) || (x == playerX && y == playerY)) {
			valid = false;
		}
	}
	if (!valid) {
		pair<int, int> foodLoc;
		foodLoc = make_pair(-100, -100);
		return foodLoc;
	}
	pair<int, int> foodLoc;
	foodLoc = make_pair(x, y);

	return foodLoc;
}

SDL_Surface* loadSurface( string path, SDL_Surface* gScreenSurface );

void EndGame(SDL_Renderer* renderer, SDL_Event event, SDL_Surface* scrSurface, SDL_Window* window) {
	while (true) {
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				exit(0);
			}
			if (event.key.keysym.scancode == SDL_SCANCODE_RETURN) {
				return;
			}
		}
	}
}


int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Event event;
	SDL_Surface* screenSurface;

	SDL_Rect player;
	player.x = 0;
	player.y = 0;
	player.h = 0;
	player.w = 0;

	int tailLength = 0;

	// Vectors for storage of tail block positions
	vector<int> tailX;
	vector<int> tailY;

	int scale = 24;
	int wScale = 24;

	// Player position
	int x = 0;
	int y = 0;
	int prevX = 0;
	int prevY = 0;

	// Movement controlsbool up = false;
	bool down = false;
	bool right = false;
	bool left = false;
    bool up = false;
	bool inputThisFrame = false;
	bool redo = false;

	// Food rectangle
	SDL_Rect food;
	food.w = scale;
	food.h = scale;
	food.x = 0;
	food.y = 0;

	pair<int, int> foodLoc = getFoodSpawn(tailX, tailY, x, y, scale, wScale, tailLength);
	food.x = foodLoc.first;
	food.y = foodLoc.second;

	window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, scale*wScale+1, scale*wScale+1, SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	float time = SDL_GetTicks() / 100;

	// Main game loop
	while (true) {

		float newTime = SDL_GetTicks() / 75; //This value (75) is the speed at which the blocks are updated
		float delta = newTime - time;
		time = newTime;

		inputThisFrame = false;

		// Check win condition, tail needs to fill all tiles
		if (tailLength >= 575) {
			EndGame(renderer, event, screenSurface, window);
			break;
		}

		// Controls
		if (SDL_PollEvent(&event)) {

			// Exit
			if (event.type == SDL_QUIT) {
				exit(0);
			}
			//Moving
			if (event.type == SDL_KEYDOWN && inputThisFrame == false) {


				if (down == false && event.key.keysym.scancode == SDL_SCANCODE_UP) {
					up = true;
					left = false;
					right = false;
					down = false;
					inputThisFrame = true;
				}
				else if (right == false && event.key.keysym.scancode == SDL_SCANCODE_LEFT) {
					up = false;
					left = true;
					right = false;
					down = false;
					inputThisFrame = true;
				}
				else if (up == false && event.key.keysym.scancode == SDL_SCANCODE_DOWN) {
					up = false;
					left = false;
					right = false;
					down = true;
					inputThisFrame = true;
				}
				else if (left == false && event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
					up = false;
					left = false;
					right = true;
					down = false;
					inputThisFrame = true;
				}

			}

		}
		// The previous position of the player block
		prevX = x;
		prevY = y;

		if (up) {
			y -= delta * scale;
		}
		else if (left) {
			x -= delta * scale;
		}
		else if (right) {
			x += delta * scale;
		}
		else if (down) {
			y += delta * scale;
		}

		// Collision detection, has played collided with food?
		if (checkCollision(food.x, food.y, x, y)) {

			// Spawn new food after it has been eaten
			foodLoc = getFoodSpawn(tailX, tailY, x, y, scale, wScale, tailLength);
			food.x = foodLoc.first;
			food.y = foodLoc.second;

			tailLength++;
		}

		// Only runs in the frames where the player block has moved
		if (delta * scale == 24) {

			// Update tail size and position
			if (tailX.size() != tailLength) {
				tailX.push_back(prevX);
				tailY.push_back(prevY);
			}
			for (int i = 0; i < tailLength; i++) {

				if (i > 0) {
					tailX[i - 1] = tailX[i];
					tailY[i - 1] = tailY[i];
				}
			}
			if (tailLength > 0) {
				tailX[tailLength - 1] = prevX;
				tailY[tailLength - 1] = prevY;
			}
		}
    //game over
		for (int i = 0; i < tailLength; i++) {

			if (x == tailX[i] && y == tailY[i]) {
				EndGame(renderer, event, screenSurface, window);
				break;

			}
		}

		// Game over if player out of bounds, also resets the game state
		if (x < 0 || y < 0 || x > scale * wScale - scale || y > scale * wScale - scale) {
			EndGame(renderer, event, screenSurface, window);
			break;
		}

		renderFood(renderer, food);
		renderPlayer(renderer, player, x, y, scale, tailX, tailY, tailLength);

		SDL_RenderPresent(renderer);
		SDL_SetRenderDrawColor(renderer, 0, 0, 61, 0);
		SDL_RenderClear(renderer);
	}
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;

}

SDL_Surface* loadSurface( string path, SDL_Surface* gScreenSurface )
{
    //The final optimized image
    SDL_Surface* optimizedSurface = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL )
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
    }
    else
    {
        //Convert surface to screen format
        optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, 0);
        if( optimizedSurface == NULL )
        {
            printf( "Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
        }

        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }
    return optimizedSurface;
}
