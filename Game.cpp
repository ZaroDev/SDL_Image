#include "Game.h"
#include <math.h>

Game::Game() {}
Game::~Game(){}

bool Game::Init()
{
	//Initialize SDL with all subsystems
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	//Create our window: title, x, y, w, h, flags
	Window = SDL_CreateWindow("Spaceship: arrow keys + space", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (Window == NULL)
	{
		SDL_Log("Unable to create window: %s", SDL_GetError());
		return false;
	}
	//Create a 2D rendering context for a window: window, device index, flags
	Renderer = SDL_CreateRenderer(Window, -1, 0);
	if (Renderer == NULL)
	{
		SDL_Log("Unable to create rendering context: %s", SDL_GetError());
		return false;
	}
	//Initialize keys array
	for (int i = 0; i < MAX_KEYS; ++i)
		keys[i] = KEY_IDLE;
	//Initialize sprites
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
	if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0)
	{
		SDL_Log("Unable to initialize images: %s", SDL_GetError());
	}
	Spaceship = IMG_Load("spaceship.png");
	SpaceshipTexture = SDL_CreateTextureFromSurface(Renderer, Spaceship);

	Shot = IMG_Load("shot.png");
	ShotTexture = SDL_CreateTextureFromSurface(Renderer, Shot);
	//Init variables
	Player.Init(20, WINDOW_HEIGHT >> 1, 104, 82, 5);
	idx_shot = 0;
	idx_shot2 = 0;

	return true;
}
void Game::Release()
{
	//Clean up all SDL initialized subsystems
	SDL_DestroyTexture(SpaceshipTexture);
	SDL_DestroyTexture(ShotTexture);
	IMG_Quit();
	SDL_Quit();
}
bool Game::Input()
{
	SDL_Event event;
	if (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)	return false;
	}

	SDL_PumpEvents();
	const Uint8* keyboard = SDL_GetKeyboardState(NULL);
	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keyboard[i])
			keys[i] = (keys[i] == KEY_IDLE) ? KEY_DOWN : KEY_REPEAT;
		else
			keys[i] = (keys[i] == KEY_REPEAT || keys[i] == KEY_DOWN) ? KEY_UP : KEY_IDLE;
	}

	return true;
}
bool Game::Update()
{
	//Read Input
	if (!Input())	return true;

	//Process Input
	int fx = 0, fy = 0;
	if (keys[SDL_SCANCODE_ESCAPE] == KEY_DOWN)	return true;
	if (keys[SDL_SCANCODE_UP] == KEY_REPEAT)	fy = -1;
	if (keys[SDL_SCANCODE_DOWN] == KEY_REPEAT)	fy = 1;
	if (keys[SDL_SCANCODE_LEFT] == KEY_REPEAT)	fx = -1;
	if (keys[SDL_SCANCODE_RIGHT] == KEY_REPEAT)	fx = 1;
	if (keys[SDL_SCANCODE_SPACE] == KEY_DOWN)
	{
		int x, y, w, h;
		Player.GetRect(&x, &y, &w, &h);
		Shots[idx_shot].Init(x + w - 75, y + h - 79, 56, 20, 10);
		Shots2[idx_shot].Init(x + w - 75, y + h - 23, 56, 20, 10);
		idx_shot++;
		idx_shot %= MAX_SHOTS;
		idx_shot2++;
		idx_shot2 %= MAX_SHOTS;
	}

	//Logic
	//Player update
	Player.Move(fx, fy);
	//Shots update
	for (int i = 0; i < MAX_SHOTS; ++i)
	{
		if (Shots[i].IsAlive())
		{
			Shots[i].Move(1, 0);
			if (Shots[i].GetX() > WINDOW_WIDTH)	Shots[i].ShutDown();
		}
		if (Shots2[i].IsAlive())
		{
			Shots2[i].Move(1, 0);
			if (Shots2[i].GetX() > WINDOW_WIDTH)	Shots2[i].ShutDown();
		}
	}
		
	return false;
}
void Game::Draw()
{
	//Set the color used for drawing operations
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
	//Clear rendering target
	SDL_RenderClear(Renderer);

	//Draw player
	SDL_Rect srcRc;
	SDL_Rect dstRc;


	srcRc.x = 0;
	srcRc.y = 0;
	srcRc.w = 104;
	srcRc.h = 82;

	Player.GetRect(&dstRc.x, &dstRc.y, &dstRc.w, &dstRc.h);
	SDL_RenderCopy(Renderer, SpaceshipTexture, &srcRc, &dstRc);

	
	//Draw shots
	SDL_Rect srcRcShot;
	SDL_Rect dstRcShot;

	srcRcShot.x = 0;
	srcRcShot.y = 0;
	srcRcShot.w = 56;
	srcRcShot.h = 20;

	SDL_SetRenderDrawColor(Renderer, 192, 0, 0, 255);
	for (int i = 0; i < MAX_SHOTS; ++i)
	{
		if (Shots[i].IsAlive())
		{
			Shots[i].GetRect(&dstRcShot.x, &dstRcShot.y, &dstRcShot.w, &dstRcShot.h);
			SDL_RenderCopy(Renderer, ShotTexture,&srcRcShot, &dstRcShot);
		}
		if (Shots2[i].IsAlive())
		{
			Shots2[i].GetRect(&dstRcShot.x, &dstRcShot.y, &dstRcShot.w, &dstRcShot.h);
			SDL_RenderCopy(Renderer, ShotTexture, &srcRcShot, &dstRcShot);
		}
	}

	//Update screen
	SDL_RenderPresent(Renderer);

	SDL_Delay(10);	// 1000/10 = 100 fps max
}