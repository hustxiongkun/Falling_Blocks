//////////////////////////////////////////////////////////////////////////////////
// Project: Game Framework
// File:    Main.cpp
//////////////////////////////////////////////////////////////////////////////////

// links
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#pragma comment(lib, "SDL2_ttf.lib")
#pragma comment(lib, "SDL2_image.lib")
#pragma comment(lib, "SDL2_mixer.lib")

// memory leak
#ifdef _DEBUG
#define  DEBUG_CLIENTBLOCK new(_CLIENT_BLOCK,__FILE__,__LINE__)
#else
#define DEBUG_CLIENTBLOCK
#endif // _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

#include <cstdio>
#include <ctime>
#include <cmath>

#include <stack>
#include <vector>

#include <SDL/SDL_mixer.h>

#include "../include/Constants.h"
#include "../include/Enums.h"
#include "../include/Tools.h"
#include "../include/Block.h"

using namespace std;

// game state struct
struct StateStruct {
	void(*StatePointer)();
};

// global data
std::stack<StateStruct> gStageStack; // stack for game state pointer
SDL_Window* gWindow = NULL; // SDL window pointer
SDL_Renderer* gRenderer = NULL; // renderer pointer
SDL_Event gEvent; // SDL event struct
int gTimer; // timer
LTexture gTextTexture;// texture for text
Mix_Music* gMusic = NULL;// music that will be played
Mix_Chunk* gWinSound = NULL;// sound effects
Mix_Chunk* gLoseSound = NULL;
Mix_Chunk* gEliminateSound = NULL;
Mix_Chunk* gCollisionSound = NULL;
Mix_Chunk* gKeydownSound = NULL;

LTexture gSprite;// texture for background image
Block* gFocusBlock = NULL;
Block* gNextBlock = NULL;
std::vector<Square*> gOldSquares;
int gScore = 0;
int gLevel = 1;
int gFocusBlockSpeed = INITIAL_SPEED;


// functions
// init and close SDL, load media
bool initSDL();
bool loadMedia();
void closeSDL();

// init and shutdown game
void init();
void shutdown();

// functions to handle states of the game
void Menu();
void Game();
void Exit();

void GameWin();
void GameLose();

// helper functions
void handleMenuInput();
void handleGameInput();
void handleExitInput();
void handleWinLoseInput();

void handleBottomCollision();
void changeFoculBlock();
int checkCompletedLindes();
void checkWin();
void checkLoss();

void drawBackground();

//collision detection
bool checkEntityCollisions(Square* square, Direction dir);
bool checkEntityCollisions(Block* block, Direction dir);
bool checkWallCollisions(Square* square, Direction dir);
bool checkWallCollisions(Block* block, Direction dir);
bool checkRotationCollisions(Block* block);


int main(int argc, char** argv) {
	// detect memory leak
	//_CrtSetBreakAlloc(1385);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);


	// start up SDL and create window
	if (!initSDL()) {
		printf("Failed to initialize!\n");
	} else {
		// load media
		if (!loadMedia()) {
			printf("Failed to load media!\n");
		} else {
			// game
			init();
			// main loop
			while (!gStageStack.empty()) {
				gStageStack.top().StatePointer();
			}

			shutdown();
		}
	}

	// free resources and close SDL
	closeSDL();

	return 0;
}

bool initSDL() {
	// initialization flag
	bool success = true;

	// initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	} else {
		// set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
			printf("Warning: Linear texture filtering not enabled!");
		}

		// create window
		gWindow = SDL_CreateWindow(WINDOW_CAPTION, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL) {
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		} else {
			// create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL) {
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			} else {
				// initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);

				// initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags)) {
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}

				// initialize SDL_ttf
				if (TTF_Init() == -1) {
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}

				// initialize SDL_mixer
				if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
				{
					printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia() {
	// loading success flag
	bool success = true;

	gTextTexture.setFont("../resources/fonts/ARIAL.TTF", 12);
	//gTextTexture.setFont("../../resources/fonts/ARIAL.TTF", 12);

	// load background image
	if (!gSprite.loadFromFile(gRenderer, "../resources/images/FallingBlocks.bmp")) {
		printf("Failed to load image!\n");
		success = false;
	} else {
		int distance = SQUARE_MEDIAN * 2;
		for (int i = 0; i < BLOCK_TOTAL; i++) {
			Block::sBlockClips[i] = { SQUARE_START_X+i*distance,SQUARE_START_Y,distance,distance };
		}
	}
	// load music
	gMusic = Mix_LoadMUS("../resources/sounds/music.wav");
	if (gMusic == NULL) {
		printf("Failed to load music! SDL mixer Error: %s\n", Mix_GetError());
		success = false;
	}
	// load sound effects
	gWinSound = Mix_LoadWAV("../resources/sounds/win.wav");
	if (gWinSound == NULL) {
		printf("Failed to load win sound effect! SDL mixer Error: %s\n", Mix_GetError());
		success = false;
	}
	gLoseSound = Mix_LoadWAV("../resources/sounds/lose.wav");
	if (gLoseSound == NULL) {
		printf("Failed to load lose sound effect! SDL mixer Error: %s\n", Mix_GetError());
		success = false;
	}
	gKeydownSound = Mix_LoadWAV("../resources/sounds/keydown.wav");
	if (gKeydownSound == NULL) {
		printf("Failed to load keydown sound effect! SDL mixer Error: %s\n", Mix_GetError());
		success = false;
	}
	gEliminateSound = Mix_LoadWAV("../resources/sounds/eliminate.wav");
	if (gEliminateSound == NULL) {
		printf("Failed to load eliminate sound effect! SDL mixer Error: %s\n", Mix_GetError());
		success = false;
	}
	gCollisionSound = Mix_LoadWAV("../resources/sounds/collision.wav");
	if (gCollisionSound == NULL) {
		printf("Failed to load collision sound effect! SDL mixer Error: %s\n", Mix_GetError());
		success = false;
	}

	return success;
}

void closeSDL() {
	// free texture
	gTextTexture.freeTexture();
	gSprite.freeTexture();

	// free the sound effects
	Mix_FreeChunk(gWinSound);
	Mix_FreeChunk(gLoseSound);
	Mix_FreeChunk(gKeydownSound);
	Mix_FreeChunk(gEliminateSound);
	Mix_FreeChunk(gCollisionSound);
	gWinSound = NULL;
	gLoseSound = NULL;
	gKeydownSound = NULL;
	gEliminateSound = NULL;
	gCollisionSound = NULL;

	// free the music
	Mix_FreeMusic(gMusic);
	gMusic = NULL;

	// destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	// quit SDL subsystems
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void init() {
	// get the number of ticks
	gTimer = SDL_GetTicks();

	// seed our random number generator
	srand(time(0));
	
	// add a pointer to exit state
	StateStruct state;
	state.StatePointer = Exit;
	gStageStack.push(state);

	// add a pointer to menu state
	state.StatePointer = Menu;
	gStageStack.push(state);

	// create block
	gFocusBlock = new Block(BLOCK_START_X, BLOCK_START_Y, &gSprite, (BlockTypes)(rand() % BLOCK_TOTAL));
	gNextBlock = new Block(NEXT_BLOCK_CIRCLE_X, NEXT_BLOCK_CIRCLE_Y, &gSprite, (BlockTypes)(rand() % BLOCK_TOTAL));
}

void shutdown() {
	// deallocate
	Square** temp1 = gFocusBlock->getSquares();
	Square** temp2 = gNextBlock->getSquares();

	for (int i = 0; i < 4; i++) {
		delete temp1[i];
		delete temp2[i];
	}
	delete gFocusBlock;
	delete gNextBlock;

	// vector saved the pointers
	for (int i = 0; i < gOldSquares.size(); i++) {
		delete gOldSquares[i];
	}
	gOldSquares.clear();
}

// game menu
void Menu() {
	// stop music
	Mix_HaltMusic();
	// control FPS
	if ((SDL_GetTicks() - gTimer) >= FRAME_RATE) {
		// handle input
		handleMenuInput();

		// clear screen
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
		SDL_RenderClear(gRenderer);

		// render
		SDL_Color textColor = { 0xFF,0xFF,0xFF };
		gTextTexture.loadFromRenderedText(gRenderer, "Start (G)ame", textColor);
		gTextTexture.render(gRenderer, 100, 150);
		gTextTexture.loadFromRenderedText(gRenderer, "(Q)uit Game", textColor);
		gTextTexture.render(gRenderer, 100, 170);

		// update
		SDL_RenderPresent(gRenderer);
		gTimer = SDL_GetTicks();
	}
}

// main game
void Game() {
	// play music
	if (Mix_PlayingMusic() == 0) {
		Mix_PlayMusic(gMusic, -1);
	}

	// force down
	static int force_down_count = 0;
	static int slider_count = SLIDE_TIME;
	// control FPS
	if ((SDL_GetTicks() - gTimer) >= FRAME_RATE) {
		handleGameInput();

		force_down_count++;// increase force down counter
		if (force_down_count >= gFocusBlockSpeed) {
			// force to move down
			if (!checkEntityCollisions(gFocusBlock, DOWN) && !checkWallCollisions(gFocusBlock, DOWN)) {
				gFocusBlock->move(DOWN);
				force_down_count = 0;
			}
		}
		// slide when focus block arrive bottom
		if (checkEntityCollisions(gFocusBlock, DOWN) || checkWallCollisions(gFocusBlock, DOWN)) {
			slider_count--;
		} else {
			slider_count = SLIDE_TIME;
		}
		if (slider_count <= 0) {
			slider_count = SLIDE_TIME;
			handleBottomCollision();
		}

		// clear screen
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
		SDL_RenderClear(gRenderer);

		// render
		// draw background
		drawBackground();
		// draw level, score and needed score text
		SDL_Color textColor = { 0,0,0 };
		gTextTexture.loadFromRenderedText(gRenderer, "Level: "+to_string(gLevel), textColor);
		gTextTexture.render(gRenderer,LEVEL_RECT_X, LEVEL_RECT_Y);
		gTextTexture.loadFromRenderedText(gRenderer, "Score: " + to_string(gScore), textColor);
		gTextTexture.render(gRenderer, SCORE_RECT_X, SCORE_RECT_Y);
		gTextTexture.loadFromRenderedText(gRenderer, "Needed: " + to_string(gLevel*POINTS_PER_LEVEL), textColor);
		gTextTexture.render(gRenderer, NEEDED_SCORE_RECT_X, NEEDED_SCORE_RECT_Y);
		// draw blocks
		gFocusBlock->draw(gRenderer);
		gNextBlock->draw(gRenderer);

		for (int i = 0; i < gOldSquares.size(); i++) {
			gOldSquares[i]->draw(gRenderer);
		}

		// update
		SDL_RenderPresent(gRenderer);
		gTimer = SDL_GetTicks();
	}
}

// exit state
void Exit() {
	// stop music
	Mix_HaltMusic();
	// control FPS
	if ((SDL_GetTicks() - gTimer) >= FRAME_RATE) {
		handleExitInput();
		// clear screen
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
		SDL_RenderClear(gRenderer);

		// render
		SDL_Color textColor = { 0xFF,0xFF,0xFF };
		gTextTexture.loadFromRenderedText(gRenderer, "Quit Game (Y or N)?", textColor);
		gTextTexture.render(gRenderer, 100, 150);

		// update
		SDL_RenderPresent(gRenderer);
		gTimer = SDL_GetTicks();
	}
}

// game win state
void GameWin() {
	// stop music
	Mix_HaltMusic();
	// control FPS
	if ((SDL_GetTicks() - gTimer) >= FRAME_RATE) {
		handleWinLoseInput();

		// clear screen
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
		SDL_RenderClear(gRenderer);

		// render
		SDL_Color textColor = { 0xFF,0xFF,0xFF };
		gTextTexture.loadFromRenderedText(gRenderer, "You Win!!!", textColor);
		gTextTexture.render(gRenderer, 100, 150);
		gTextTexture.loadFromRenderedText(gRenderer, "Quit Game (Y or N)?", textColor);
		gTextTexture.render(gRenderer, 100, 170);

		// update
		SDL_RenderPresent(gRenderer);
		gTimer = SDL_GetTicks();
	}
}

// game lose state
void GameLose() {
	// stop music
	Mix_HaltMusic();
	// control FPS
	if ((SDL_GetTicks() - gTimer) >= FRAME_RATE) {
		handleWinLoseInput();

		// clear screen
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
		SDL_RenderClear(gRenderer);

		// render
		SDL_Color textColor = { 0xFF,0xFF,0xFF };
		gTextTexture.loadFromRenderedText(gRenderer, "You Lose.", textColor);
		gTextTexture.render(gRenderer, 120, 150);
		gTextTexture.loadFromRenderedText(gRenderer, "Quit Game (Y or N)?", textColor);
		gTextTexture.render(gRenderer, 120, 170);

		// update
		SDL_RenderPresent(gRenderer);
		gTimer = SDL_GetTicks();
	}
}

// receive input handle it for menu state
void handleMenuInput() {
	// get event information
	while (SDL_PollEvent(&gEvent) != 0) {
		// handle user manually closing game window
		if (gEvent.type == SDL_QUIT) {
			// pop all state
			while (!gStageStack.empty()) {
				gStageStack.pop();
			}
			return;// game is over, exit the function
		}
		// handle keyboard input
		if (gEvent.type == SDL_KEYDOWN) {
			switch (gEvent.key.keysym.sym)
			{
			case SDLK_ESCAPE:
			case SDLK_q:
				gStageStack.pop();
				return;// this state is done, exit the function
				break;
			case SDLK_g:
				StateStruct temp;
				temp.StatePointer = Game;// add a pointer to game state
				gStageStack.push(temp);
				return;// this state is done, exit the function
				break;
			default:
				break;
			}
		}
	}
}

// receive input handle it for main game
void handleGameInput() {
	// get event information
	while (SDL_PollEvent(&gEvent) != 0) {
		// handle user manually closing game window
		if (gEvent.type == SDL_QUIT) {
			// pop all state
			while (!gStageStack.empty()) {
				gStageStack.pop();
			}
			return;// game is over, exit the function
		}
		// handle keyboard input
		if (gEvent.type == SDL_KEYDOWN) {
			switch (gEvent.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				gStageStack.pop();
				return;// this state is done, exit the function
				break;
			case SDLK_UP:
				if (!checkRotationCollisions(gFocusBlock)) {
					gFocusBlock->rotate();
					// play effect
					Mix_PlayChannel(-1, gKeydownSound, 0);
				} else {
					// play effect
					Mix_PlayChannel(-1, gCollisionSound, 0);
				}
				break;
			case SDLK_DOWN:
				if (!checkEntityCollisions(gFocusBlock, DOWN) && !checkWallCollisions(gFocusBlock, DOWN)) {
					gFocusBlock->move(DOWN);
					// play effect
					Mix_PlayChannel(-1, gKeydownSound, 0);
				} 
				break;
			case SDLK_LEFT:
				if (!checkEntityCollisions(gFocusBlock, LEFT) && !checkWallCollisions(gFocusBlock, LEFT)) {
					gFocusBlock->move(LEFT);
					// play effect
					Mix_PlayChannel(-1, gKeydownSound, 0);
				} else {
					// play effect
					Mix_PlayChannel(-1, gCollisionSound, 0);
				}
				break;
			case SDLK_RIGHT:
				if (!checkEntityCollisions(gFocusBlock, RIGHT) && !checkWallCollisions(gFocusBlock, RIGHT)) {
					gFocusBlock->move(RIGHT);
					// play effect
					Mix_PlayChannel(-1, gKeydownSound, 0);
				} else {
					// play effect
					Mix_PlayChannel(-1, gCollisionSound, 0);
				}
				break;
			default:
				break;
			}
		}
	}
}

// receive input handle it for exit state
void handleExitInput() {
	// get event information
	while (SDL_PollEvent(&gEvent) != 0) {
		// handle user manually closing game window
		if (gEvent.type == SDL_QUIT) {
			// pop all state
			while (!gStageStack.empty()) {
				gStageStack.pop();
			}
			return;// game is over, exit the function
		}
		// handle keyboard input
		if (gEvent.type == SDL_KEYDOWN) {
			switch (gEvent.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				gStageStack.pop();
				return;// this state is done, exit the function
				break;
			// Yes
			case SDLK_y:
				gStageStack.pop();
				return;// game is over, exit the function
				break;
			// No
			case SDLK_n:
				StateStruct temp;
				temp.StatePointer = Menu;
				gStageStack.push(temp);
				return;// this state is done, exit the function
				break;
			default:
				break;
			}
		}
	}
}

// handle win or lose state input
void handleWinLoseInput() {
	// get event information
	while (SDL_PollEvent(&gEvent) != 0) {
		// handle user manually closing game window
		if (gEvent.type == SDL_QUIT) {
			// pop all state
			while (!gStageStack.empty()) {
				gStageStack.pop();
			}
			return;// game is over, exit the function
		}
		// handle keyboard input
		if (gEvent.type == SDL_KEYDOWN) {
			switch (gEvent.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				gStageStack.pop();
				return;// this state is done, exit the function
				break;
				// Yes
			case SDLK_y:
				gStageStack.pop();
				return;// game is over, exit the function
				break;
				// No
			case SDLK_n:
				gStageStack.pop();
				// new game exit and menu
				StateStruct temp;
				temp.StatePointer = Exit;
				gStageStack.push(temp);
				temp.StatePointer = Menu;
				gStageStack.push(temp);
				return;// this state is done, exit the function
				break;
			default:
				break;
			}
		}
	}
}

void handleBottomCollision() {
	changeFoculBlock();

	// get completed line number
	int lineNums = checkCompletedLindes();
	if (lineNums > 0) {
		// play effect
		Mix_PlayChannel(-1, gEliminateSound, 0);
		// increase score by line number
		gScore += lineNums*POINTS_PER_LINE;
		// check whether if change level
		if (gScore >= gLevel*POINTS_PER_LEVEL) {
			gLevel++;
			gFocusBlockSpeed -= SPEED_CHANGE;
			checkWin();
		}
	}
	checkLoss();
}

void changeFoculBlock() {
	Square** squares = gFocusBlock->getSquares();
	// add squares into vector gOldSquares
	for (int i = 0; i < 4; i++) {
		gOldSquares.push_back(squares[i]);
	}
	// change block
	delete gFocusBlock;
	gFocusBlock = gNextBlock;
	gFocusBlock->setupSquares(BLOCK_START_X, BLOCK_START_Y);

	// create new next block
	gNextBlock = new Block(NEXT_BLOCK_CIRCLE_X, NEXT_BLOCK_CIRCLE_Y, &gSprite, (BlockTypes)(rand() % BLOCK_TOTAL));
}

int checkCompletedLindes() {
	int lineNums = 0;

	// all game area have 13 lines
	int line[13] = { 0 };// per line has how many squares
	int accumulationLines[13] = {0};// per line and before it has how many completed lines
	int distance = SQUARE_MEDIAN * 2;
	// bottom and top square center hight
	int bottom = GAME_AREA_BOTTOM - SQUARE_MEDIAN;
	int top = bottom - 12 * distance;

	int row;
	// count squares in line
	for (int i = 0; i < gOldSquares.size(); i++) {
		row = (gOldSquares[i]->getCenterY() - top) / distance;
		line[row]++;
	}
	// count completed lines
	for (row = 0; row < 13; row++) {
		if (line[row] == SQUARES_PER_ROW) {
			lineNums++;
		}
		accumulationLines[row] = lineNums;
	}
	// delete completed lines
	for (int i = 0; i < gOldSquares.size(); i++) {
		row = (gOldSquares[i]->getCenterY() - top) / distance;
		// check if it is in a completed lines
		if (line[row] == SQUARES_PER_ROW) {
			delete gOldSquares[i];
			gOldSquares.erase(gOldSquares.begin() + i);
			i--;// for erase a element
		} else {
			// move down
			int y = gOldSquares[i]->getCenterY();
			y += (lineNums - accumulationLines[row])*distance;
			gOldSquares[i]->setCenterY(y);
		}
	}

	return lineNums;
}

void checkWin() {
	if (gLevel > LEVEL_NUMS) {
		// clear entity
		for (int i = 0; i < gOldSquares.size(); i++) {
			delete gOldSquares[i];
		}
		gOldSquares.clear();
		// set score and level
		gScore = 0;
		gLevel = 1;
		gFocusBlockSpeed = INITIAL_SPEED;
		// clear game state
		while (!gStageStack.empty()) {
			gStageStack.pop();
		}
		StateStruct winState;
		winState.StatePointer = GameWin;
		gStageStack.push(winState);
		// play effect
		Mix_PlayChannel(-1, gWinSound, 0);
	}
}

void checkLoss() {
	if (checkEntityCollisions(gFocusBlock, DOWN)) {
		// clear entity
		for (int i = 0; i < gOldSquares.size(); i++) {
			delete gOldSquares[i];
		}
		gOldSquares.clear();
		// set score and level
		gScore = 0;
		gLevel = 1;
		gFocusBlockSpeed = INITIAL_SPEED;
		// clear game state
		while (!gStageStack.empty()) {
			gStageStack.pop();
		}
		
		StateStruct lossState;
		lossState.StatePointer = GameLose;
		gStageStack.push(lossState);
		// play effect
		Mix_PlayChannel(-1, gLoseSound, 0);
	}
}

void drawBackground() {
	SDL_Rect clip;
	// select clip rect according to level
	switch (gLevel)
	{
	case 1:
		clip = { LEVEL_ONE_X,LEVEL_ONE_Y,WINDOW_WIDTH,WINDOW_HEIGHT };
		break;
	case 2:
		clip = { LEVEL_TWO_X,LEVEL_TWO_Y,WINDOW_WIDTH,WINDOW_HEIGHT };
		break;
	case 3:
		clip = { LEVEL_THREE_X,LEVEL_THREE_Y,WINDOW_WIDTH,WINDOW_HEIGHT };
		break;
	case 4:
		clip = { LEVEL_FOUR_X,LEVEL_FOUR_Y,WINDOW_WIDTH,WINDOW_HEIGHT };
		break;
	case 5:
		clip = { LEVEL_FIVE_X,LEVEL_FIVE_Y,WINDOW_WIDTH,WINDOW_HEIGHT };
		break;
	default:
		break;
	}
	// render background
	gSprite.render(gRenderer, 0, 0, &clip);
}

bool checkEntityCollisions(Square* square, Direction dir) {
	int x = square->getCenterX();
	int y = square->getCenterY();
	int distance = SQUARE_MEDIAN * 2;

	// get position after move on dir
	switch (dir)
	{
	case LEFT:
		x -= distance;
		break;
	case RIGHT:
		x += distance;
		break;
	case DOWN:
		y += distance;
		break;
	default:
		break;
	}
	// check
	for (int i = 0; i < gOldSquares.size(); i++) {
		if (abs(x - gOldSquares[i]->getCenterX()) < distance && abs(y - gOldSquares[i]->getCenterY()) < distance) {
			return true;
		}
	}
	return false;
}

bool checkEntityCollisions(Block* block, Direction dir) {
	Square** squares = block->getSquares();
	for (int i = 0; i < 4; i++) {
		if (checkEntityCollisions(squares[i], dir)) {
			return true;
		}
		
	}
	return false;
}

bool checkWallCollisions(Square* square, Direction dir) {
	int x = square->getCenterX();
	int y = square->getCenterY();
	int distance = SQUARE_MEDIAN * 2;

	// check
	switch (dir)
	{
	case LEFT:
		if (x - distance < GAME_AREA_LEFT) {
			return true;
		} else {
			return false;
		}
		break;
	case RIGHT:
		if (x + distance > GAME_AREA_RIGHT) {
			return true;
		} else {
			return false;
		}
		break;
	case DOWN:
		if (y + distance > GAME_AREA_BOTTOM) {
			return true;
		} else {
			return false;
		}
		break;
	default:
		break;
	}
	return false;
}

bool checkWallCollisions(Block* block, Direction dir) {
	Square** squares = block->getSquares();
	for (int i = 0; i < 4; i++) {
		if (checkWallCollisions(squares[i], dir)) {
			return true;
		}

	}
	return false;
}

bool checkRotationCollisions(Block* block) {
	// get positions after rotation
	int* temp = block->getRotatePosition();
	int x, y;
	int distance = SQUARE_MEDIAN * 2;
	for (int i = 0; i < 4; i++) {
		x = temp[2 * i];
		y = temp[2 * i + 1];
		// check wall
		if (x < GAME_AREA_LEFT || x > GAME_AREA_RIGHT || y > GAME_AREA_BOTTOM) {
			delete temp;
			return true;
		}
		// check entity
		for (int j = 0; j < gOldSquares.size(); j++) {
			if (abs(x - gOldSquares[j]->getCenterX()) < distance && abs(y - gOldSquares[j]->getCenterY()) < distance) {
				delete temp;
				return true;
			}
		}
	}
	delete temp;
	return false;
}