#include <SDL.h>
#include <SDL_ttf.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <time.h>
#include <SDL_image.h>
#include "Board.h"
#include "Button.h"
#include "Top10.h"

int width = 0;
int height = 0;

// Affichage d'une chaîne de caractères
void printStr(SDL_Renderer* r, TTF_Font* pFont, SDL_Color* pColor, const char *text, int x, int y)
{
	if (pFont != NULL)
	{
		SDL_Surface* textSurface = TTF_RenderText_Solid(pFont, text, *pColor);
		if (textSurface != NULL)
		{
			SDL_Texture* message = SDL_CreateTextureFromSurface(r, textSurface);
			if (x == TEXT_CENTER)
				x = width / 2 - textSurface->w / 2;
			SDL_Rect textRect = { x, y, textSurface->w, textSurface->h };
			SDL_RenderCopy(r, message, NULL, &textRect);
			SDL_FreeSurface(textSurface);
			SDL_DestroyTexture(message);
		}
	}
}

// Menu initial pour lancer le jeu ou quitter
// Retourne '1' pour démarrer le jeu et '2' pour quitter
char mainMenu(SDL_Renderer* renderer, Button *start, Button *exit) {
	// Affichage
	start->select(true);
	SDL_RenderPresent(renderer);
	bool quit = false;
	while (!quit) {
		SDL_Event event;
		//Traiter les évènements
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				// Alt F4
			case SDL_QUIT:
				return '2';
				break;
				// Gestion du clavier
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_LEFT:
					start->select(true);
					exit->select(false);
					SDL_RenderPresent(renderer);
					break;
				case SDLK_RIGHT:
					start->select(false);
					exit->select(true);
					SDL_RenderPresent(renderer);
					break;
				case SDLK_KP_ENTER:
				case SDLK_RETURN:
					quit = true;
					break;
				default:
					break;
				}
				// Gestion de la souris
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT) {
					if (start->isIn(event.motion.x, event.motion.y)) {
						start->select(true);
						exit->select(false);
						SDL_RenderPresent(renderer);
					}
					if (exit->isIn(event.motion.x, event.motion.y)) {
						exit->select(true);
						start->select(false);
						SDL_RenderPresent(renderer);
					}
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					if (start->isIn(event.motion.x, event.motion.y) && start->isSelected())
						quit = true;
					if (exit->isIn(event.motion.x, event.motion.y) && exit->isSelected())
						quit = true;
				}
				break;
			}
		}
	}
	if (start->isSelected())
		return '1';
	return '2';
}

// Saisie du nom du joueur dans le top10
// Retourne false si l'utilisateur quitte sans sauvegarder
bool getPlayerName(SDL_Renderer* renderer, TTF_Font* pFont, int x, int y, char* player) {
	int lenght = 0;
	int color = 0;
	SDL_Color colors[2] = { { 255, 255, 255, 255 },{ 0, 0, 0, 255 } };
	SDL_Rect rectangle = { x, y, 11, 18 };
	Uint32 refTicks = SDL_GetTicks();
	char cKey[2];
	cKey[1] = '\0';
	while (cKey[0] != '\n') {
		SDL_Event event;
		Uint32 currentTicks = SDL_GetTicks() - refTicks;
		if (currentTicks > 600) {
			SDL_SetRenderDrawColor(renderer, colors[color].r, colors[color].g, colors[color].b, colors[color].a);
			color = (color + 1) % 2;
			rectangle.x = x + (lenght + 1) * 11;
			SDL_RenderFillRect(renderer, &rectangle);
			SDL_RenderPresent(renderer);
			refTicks = SDL_GetTicks();
		}
		//Traiter les évènements
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				// Alt F4
			case SDL_QUIT:
				return false;
				break;
				// Gestion du clavier
			case SDL_KEYDOWN:
				SDL_Keycode key = event.key.keysym.sym;
				// Caractère alpha numérique(lettres + chiffres) + '-'
				if (((key >= SDLK_a && key <= SDLK_z)
					|| (key >= SDLK_0 && key <= SDLK_9)
					|| (key >= SDLK_KP_1 && key <= SDLK_KP_0)
					|| key == SDLK_KP_MINUS) && lenght < 12) {
					int caps = -1;
					// Gestion des touches shift et capslock(majuscule en continu) du clavier
					SDL_Keymod keymod = SDL_GetModState();
					if (keymod & KMOD_CAPS)
						caps = 1;
					if (keymod & KMOD_SHIFT)
						caps *= -1;
					cKey[0] = '\0';
					if (key >= SDLK_KP_1 && key <= SDLK_KP_0) {
						cKey[0] = (key - SDLK_KP_1) + '1';
						if (cKey[0] > '9')
							cKey[0] = '0';
					}
					if (key >= SDLK_0 && key <= SDLK_9 && caps > 0)
						cKey[0] = key;
					if (key == SDLK_KP_MINUS || (caps < 0 && key == SDLK_6))
						cKey[0] = '-';
					if (key >= SDLK_a && key <= SDLK_z) 
						if (caps > 0)
							cKey[0] = key - ('a' - 'A');
						else
							cKey[0] = key;
					if (cKey[0] > '\0') {
						player[lenght] = cKey[0];
						lenght++;
						player[lenght] = '\0';
						SDL_Surface* textSurface = TTF_RenderText_Solid(pFont, cKey, colors[0]);
						if (textSurface != NULL)
						{
							if (color > 0) {
								SDL_SetRenderDrawColor(renderer, colors[color].r, colors[color].g, colors[color].b, colors[color].a);
								rectangle.x = x + lenght * 11;
								SDL_RenderFillRect(renderer, &rectangle);
							}
							SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, textSurface);
							SDL_Rect textRect = { x + lenght * 11, y, textSurface->w, textSurface->h };
							SDL_RenderCopy(renderer, message, NULL, &textRect);
							SDL_FreeSurface(textSurface);
							SDL_DestroyTexture(message);
							SDL_RenderPresent(renderer);
						}
					}
				}
				switch (key) {
				case SDLK_KP_ENTER:
				case SDLK_RETURN:
					if (lenght > 0)
						cKey[0] = '\n';
					break;
				case SDLK_BACKSPACE:
					if (lenght > 0) {
						lenght--;
						player[lenght] = '\0';
						if (color > 0) {
							SDL_SetRenderDrawColor(renderer, colors[color].r, colors[color].g, colors[color].b, colors[color].a);
							rectangle.x = x + (lenght + 2) * 11;
							SDL_RenderFillRect(renderer, &rectangle);
						}
						refTicks -= 1000;
					}
					break;
				default:
					break;
				}
			}
		}
	}
	return true;
}

int main(int argc, char *argv[])
{
	char currentDir[280];
	char fileName[280];
	// Récupération du répertoire courrant pour lecture des différents fichiers
	GetCurrentDirectoryA(250, currentDir);
	srand(time(NULL));
	if (SDL_VideoInit(NULL) < 0) // Initialisation de la SDL
	{
		printf("Erreur d'initialisation de la SDL : %s", SDL_GetError());
		return EXIT_FAILURE;
	}

	// Création de la fenêtre :
	SDL_Window* window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 750, 850, 0); // SDL_WINDOW_RESIZABLE);
	if (window == NULL)
	{
		printf("Erreur lors de la creation d'une fenetre : %s", SDL_GetError());
		return EXIT_FAILURE;
	}
	strcpy_s(fileName, currentDir);
	strcat_s(fileName, "\\Tetris.ico");
	SDL_SetWindowIcon(window, SDL_LoadBMP(fileName));

	// Création du renderer SDL :
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL)
	{
		printf("Erreur lors de la creation d'un renderer : %s", SDL_GetError());
		return EXIT_FAILURE;
	}

	// Initialisation de la fonte de caractère
	TTF_Init();
	TTF_Font* pFont = TTF_OpenFont("c:/windows/fonts/cour.ttf", 16);	// charge la fonte de caractères
	TTF_SetFontStyle(pFont, TTF_STYLE_BOLD);

	SDL_GetWindowSize(window, &width, &height);
	SDL_Rect rectGameOver = { 100 - 3, 200, 356, 477 };
	SDL_Rect rectStart = { SCORE_X - 30, SCORE_Y + 500, 100, 100 };
	SDL_Rect rectExit = { SCORE_X + 95, SCORE_Y + 500, 100, 100 };

	strcpy_s(fileName, currentDir);
	strcat_s(fileName, "\\Start.jpg");
	Button start(renderer, fileName, SCORE_X - 30, SCORE_Y + 500);
	strcpy_s(fileName, currentDir);
	strcat_s(fileName, "\\Exit.jpg");
	Button exit(renderer, fileName, SCORE_X + 95, SCORE_Y + 500);
	strcpy_s(fileName, currentDir);
	strcat_s(fileName, "\\Top10.dat");

	// Création de l'objet top10 (lecture du fichier Top10.dat)
	Top10 top10(fileName, "TetrisTX");
	top10.load();

	// Initialisation de l'objet board qui correspond au plateau de jeu
	Board board(window, renderer, top10.highScore());

	bool quit = false;
	int score = 0;
	// Boucle principale
	while (!quit) {
		board.init();
		// Affichage des boutons lancer et quitter le jeu
		start.draw();
		exit.draw();
		SDL_RenderPresent(renderer);
		// Attente du choix de l'utilisateur
		char choice = mainMenu(renderer, &start, &exit);
		if (choice == '1') { // Lancement du jeu
			score = board.start();
			strcpy_s(fileName, currentDir);
			strcat_s(fileName, "\\GameOver.jpg");
			// Affichage de l'image GameOver
			SDL_Surface *gameOver = IMG_Load(fileName);
			SDL_Texture *jpg = SDL_CreateTextureFromSurface(renderer, gameOver);
			SDL_RenderCopy(renderer, jpg, NULL, &rectGameOver);
			SDL_FreeSurface(gameOver);
			SDL_DestroyTexture(jpg);
			if (score >= 0) { // Si l'utilisateur n'a pas demandé à quitter
				if (top10.isIn(score) > 0) { // Si le score est dans le top 10
					int y = board.drawTop10(&top10, top10.isIn(score), score);
					SDL_RenderPresent(renderer);
					char playerName[32] = "";
					// Saisie du nom du joueur
					if (getPlayerName(renderer, pFont, SCORE_X - 18, y, playerName)) {
						top10.insert(playerName, score);
						// Sauvegarde du nouveau top 10
						if (top10.save())
							board.drawTop10(&top10, - 1, 0);
					}
					else
						quit = true;
				}
			}
		}
		else
			quit = true;
	}

	// Destruction du renderer et de la fenêtre :
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit(); // On quitte la SDL
}