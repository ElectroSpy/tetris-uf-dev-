#include <string.h>
#include <stdlib.h>
#include <Windows.h>
#include <stdio.h>
#include "Board.h"
#include "Button.h"
#include "ButtonOnOff.h"

// Initialisation du plateau de jeu
Board::Board(SDL_Window* window, SDL_Renderer* rd, int highScore)
{
	_currentPiece = NULL;
	_nextPiece = NULL;
	_pFont = TTF_OpenFont("c:/windows/fonts/cour.ttf", 16);	// charge la fonte de caractères
	TTF_SetFontStyle(_pFont, TTF_STYLE_BOLD);
	_window = window;
	_renderer = rd;
	_x = 100;
	_y = 40;
	_highScore = highScore;
	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
	SDL_RenderClear(_renderer); // Fenêtre en fond noir
	SDL_SetRenderDrawColor(_renderer, 200, 200, 200, 255);
	SDL_Rect rectangle = { _x - BORDER_WIDTH * 2, _y, PIECE_SIZE * GRID_WIDTH + BORDER_WIDTH * 4, PIECE_SIZE * GRID_HEIGH + BORDER_WIDTH * 2 };
	SDL_RenderFillRect(_renderer, &rectangle);
	SDL_RenderPresent(_renderer);
	_draw();
	_drawScore();
	FMOD_RESULT resultat;
	FMOD_System_Create(&_sndSystem);
	FMOD_System_Init(_sndSystem, 1, FMOD_INIT_NORMAL, NULL);
	/* Chargement du son et vérification du chargement */
	resultat = FMOD_System_CreateSound(_sndSystem, "FullLine.wav", FMOD_CREATESAMPLE, 0, &_sndFullLine);
	if (resultat != FMOD_OK)
		_sndFullLine = NULL;
	resultat = FMOD_System_CreateSound(_sndSystem, "DropDown.wav", FMOD_CREATESAMPLE, 0, &_sndDropDown);
	if (resultat != FMOD_OK)
		_sndDropDown = NULL;
	resultat = FMOD_System_CreateSound(_sndSystem, "GameOver.wav", FMOD_CREATESAMPLE, 0, &_sndGameOver);
	if (resultat != FMOD_OK)
		_sndGameOver = NULL;
	resultat = FMOD_System_CreateSound(_sndSystem, "Thunder.wav", FMOD_CREATESAMPLE, 0, &_sndThunder);
	if (resultat != FMOD_OK)
		_sndThunder = NULL;
}

Board::~Board()
{
	/* On libère le son et on ferme et libère l'objet système */
	if (_sndFullLine != NULL)
		FMOD_Sound_Release(_sndFullLine);
	if (_sndDropDown != NULL)
		FMOD_Sound_Release(_sndDropDown);
	if (_sndGameOver != NULL)
		FMOD_Sound_Release(_sndGameOver);
	if (_sndThunder != NULL)
		FMOD_Sound_Release(_sndThunder);

	FMOD_System_Close(_sndSystem);
	FMOD_System_Release(_sndSystem);

	if (_currentPiece != NULL)
		delete _currentPiece;
	if (_nextPiece != NULL)
		delete _nextPiece;
	TTF_CloseFont(_pFont);
}

// Permet de relancer une partie
void Board::init() {
	if (_currentPiece != NULL)
		delete _currentPiece;
	if (_nextPiece != NULL)
		delete _nextPiece;
	_currentPiece = NULL;
	_nextPiece = NULL;
	memset(_grid, 0, sizeof(_grid));
	for (int y = 0; y < GRID_HEIGH; y++) {
		_grid[0][y] = ' ';
		_grid[GRID_WIDTH + 1][y] = ' ';
	}
	for (int x = 0; x < GRID_WIDTH + 2; x++)
		_grid[x][GRID_HEIGH] = ' ';
	_score = 0;
	_level = 1;
	_lines = 0;
	_width = 0;
	_height = 0;
}

// Lancement d'une partie, retourne le score
int Board::start() {
	_currentPiece = new Piece(rand() % 7);
	_nextPiece = new Piece(rand() % 7);
	bool quit = false;
	bool refresh = false;
	bool newPiece = false;
	bool downMode = false;
	bool downModeMove = false;
	bool pauseMode = false;
	bool soundsMode = false;
	int w = 0;
	int h = 0;
	_drawScore();
	char fileName[280];
	GetCurrentDirectoryA(250, fileName);
	strcat_s(fileName, "\\Pause.jpg");
	Button button(_renderer, fileName, SCORE_X + 30, SCORE_Y + 500);
	GetCurrentDirectoryA(250, fileName);
	strcat_s(fileName, "\\Speaker");
	ButtonOnOff buttonOnOff(_renderer, fileName, SCORE_X + 30, SCORE_Y + 500);

	FMOD_CHANNEL *channel; // Nécessaire pour jouer un son
	FMOD_System_GetChannel(_sndSystem, 1, &channel);
	if (buttonOnOff.status())
		FMOD_System_PlaySound(_sndSystem, _sndThunder, 0, 0, &channel);

	Uint32 refTicks = SDL_GetTicks(); // Gestion du temps
	// boucle principale
	while (!quit)
	{
		SDL_GetWindowSize(_window, &w, &h);
		if (w != _width || h != _height || refresh)
		{
			_width = w;
			_height = h;
			refresh = false;
			_draw();

			// Affichage
			SDL_RenderPresent(_renderer);
		}
		Uint32 currentTicks = SDL_GetTicks() - refTicks;
		if (pauseMode) { // Faire clignoter le bouton pause
			if (currentTicks > 1000) {
				button.select(!button.isSelected());
				SDL_RenderPresent(_renderer);
				refTicks = SDL_GetTicks();
			}
		}
		else
			if (!soundsMode)
				if (currentTicks > 1000 - 100 * _level || (downMode && !newPiece && currentTicks > 1)) {
					refTicks = SDL_GetTicks();
					// Gestion du mouvement des pièces en fonction du temps
					if (newPiece)
						if (!_currentPiece->down((const char(*)[])_grid, GRID_WIDTH + 2, GRID_HEIGH + 1)) {
						_currentPiece->insert((char(*)[])_grid, GRID_WIDTH + 2, GRID_HEIGH + 1);
						_score += 10;
						for (int y = GRID_HEIGH - 1; y > 0; y--) {
							bool full = true;
							for (int x = 1; x < GRID_WIDTH + 1; x++)
								if (_grid[x][y] == 0) {
									full = false;
									break;
								}
							if (full) {
								for (int y1 = y - 1; y1 >= 0; y1--)
									for (int x = 1; x < GRID_WIDTH + 1; x++)
										_grid[x][y1 + 1] = _grid[x][y1];
								for (int x = 1; x < GRID_WIDTH + 1; x++)
									_grid[x][0] = 0;
								_lines++;
								if (_lines % 10 == 0)
									_level++;
								_score += 100;
								if (buttonOnOff.status())
									FMOD_System_PlaySound(_sndSystem, _sndFullLine, 0, 0, &channel);
								y++;
								}
						}
						delete _currentPiece;
						_currentPiece = _nextPiece;
						_nextPiece = new Piece(rand() % 7);
						newPiece = false;
						downMode = false;
						downModeMove = false;
						_drawScore();
						if (!_nextPiece->check((const char(*)[])_grid, GRID_WIDTH + 2, GRID_HEIGH + 1)) {
							_draw(true);
							SDL_RenderPresent(_renderer);
							break;
						}
					}
					else {
						newPiece = false;
						downMode = false;
						downModeMove = false;
					}
					else {
						refresh = _currentPiece->down((const char(*)[])_grid, GRID_WIDTH + 2, GRID_HEIGH + 1);
						if (!refresh) {
							newPiece = true;
							if (buttonOnOff.status())
								FMOD_System_PlaySound(_sndSystem, _sndDropDown, 0, 0, &channel);
						}
					}
					refresh = true;
				}

		SDL_Event event;
		//Traiter les évènements
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				// Alt F4
			case SDL_QUIT:
				quit = true;
				break;
				// Gestion du clavier
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					if (!pauseMode && !soundsMode)
						quit = true;
					break;
				case SDLK_RIGHT:
					if (!pauseMode && !soundsMode) {
						if (!downModeMove)
							refresh = _currentPiece->right((const char(*)[])_grid, GRID_WIDTH + 2, GRID_HEIGH + 1);
						if (downMode && newPiece && !downModeMove)
							downModeMove = true;
					}
					break;
				case SDLK_LEFT:
					if (!pauseMode && !soundsMode) {
						if (!downModeMove)
							refresh = _currentPiece->left((const char(*)[])_grid, GRID_WIDTH + 2, GRID_HEIGH + 1);
						if (downMode && newPiece && !downModeMove)
							downModeMove = true;
					}
					break;
				case SDLK_UP:
					if (!pauseMode && !soundsMode) {
						if (!downModeMove)
							refresh = _currentPiece->turn((const char(*)[])_grid, GRID_WIDTH + 2, GRID_HEIGH + 1);
						if (downMode && newPiece && !downModeMove)
							downModeMove = true;
					}
					break;
				case SDLK_DOWN:
					if (!pauseMode && !downMode && !soundsMode) {
						downMode = true;
						_score += (GRID_HEIGH - _currentPiece->y()) / 2;
					}
					break;
				case SDLK_p: // Mode pause
				case SDLK_SPACE:
					if (pauseMode) {
						pauseMode = false;
						_drawScore();
					}
					else
						if (!soundsMode) {
							pauseMode = true;
							SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
							SDL_Rect rectangle = { SCORE_X, SCORE_Y + 400, SCORE_WIDTH, 200 };
							SDL_RenderFillRect(_renderer, &rectangle);
							button.draw();
							SDL_RenderPresent(_renderer);
						}
					break;
				case SDLK_KP_ENTER:
				case SDLK_RETURN:
					if (soundsMode)
						buttonOnOff.select();
					break;
				case SDLK_s: // Mode sounds (permet de couper ou d'activer le son)
					if (soundsMode) { 
						soundsMode = false;
						_drawScore();
					}
					else 
						if (!pauseMode) {
							soundsMode = true;
							SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
							SDL_Rect rectangle = { SCORE_X, SCORE_Y + 400, SCORE_WIDTH, 200 };
							SDL_RenderFillRect(_renderer, &rectangle);
							buttonOnOff.draw();
							SDL_RenderPresent(_renderer);
						}
					break;
				default:
					break;
				}
				// Gestion de la souris pour le mode pause et le mode sounds
			case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_LEFT)
					if (pauseMode && button.isIn(event.motion.x, event.motion.y))
					{
						pauseMode = false;
						_drawScore();
					}
					else
						if (soundsMode && buttonOnOff.isIn(event.motion.x, event.motion.y))
							buttonOnOff.select();
				break;
			}
		}
	}

	if (buttonOnOff.status())
		FMOD_System_PlaySound(_sndSystem, _sndGameOver, 0, 0, &channel);

	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
	SDL_Rect rectangle = { SCORE_X - 50, SCORE_Y + 400, SCORE_WIDTH + 100, 220 };
	SDL_RenderFillRect(_renderer, &rectangle);
	SDL_RenderPresent(_renderer);

	if (quit)
		return -1;
	return _score;
}

// Affiche une chaîne de caractère
void Board::_printStr(SDL_Color* pColor, const char *text, int x, int y, int position) const
{
	SDL_Surface* textSurface = TTF_RenderText_Solid(_pFont, text, *pColor);
	if (textSurface != NULL)
	{
		SDL_Texture* message = SDL_CreateTextureFromSurface(_renderer, textSurface);
		if (position == TEXT_CENTER)
			x = (SCORE_WIDTH - textSurface->w) / 2 + x;
		SDL_Rect textRect = { x, y, textSurface->w, textSurface->h };
		SDL_RenderCopy(_renderer, message, NULL, &textRect);
		SDL_FreeSurface(textSurface);
		SDL_DestroyTexture(message);
	}
}

// Affiche le top 10
int Board::drawTop10(const Top10* top10, int index, int score) const {
	int ret = 0;
	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
	SDL_Rect rectangle = { SCORE_X - 50, SCORE_Y, SCORE_WIDTH + 100, 620 };
	SDL_RenderFillRect(_renderer, &rectangle);
	SDL_Color pink = { 102, 0, 51, 255 };
	SDL_Color yellowlight = { 255, 255, 102, 255 };
	SDL_Color white = { 255, 255, 255, 255 };
	_printStr(&pink, "High Score", SCORE_X + 30, SCORE_Y);
	_printStr(&pink, "    Name          Score", SCORE_X - 50, SCORE_Y + 20);
	_printStr(&pink, "_______________________", SCORE_X - 50, SCORE_Y + 30);
	int shift = 0;
	for (int i = 1; i <= 10; i++) {
		SDL_Color* pColor = &yellowlight;
		char buffer[30];
		char line[80];
		sprintf_s(buffer, "%d. ", i);
		if (i < 10)
			strcpy_s(line, " ");
		else
			strcpy_s(line, "");
		strcat_s(line, buffer);
		if (i == index) {
			pColor = &white;
			strcpy_s(buffer, "");
			shift = -1;
			ret = SCORE_Y + 50 + i * 22;
		}
		else
			strcpy_s(buffer, top10->playerName(i + shift));
		for (int j = strlen(buffer); j < 13; j++)
			strcat_s(buffer, " ");
		strcat_s(line, buffer);
		if (i == index)
			_itoa_s(score, buffer, 10);
		else
			_itoa_s(top10->score(i + shift), buffer, 10);
		for (int j = strlen(buffer); j < 6; j++)
			strcat_s(line, " ");
		strcat_s(line, buffer);
		_printStr(pColor, line, SCORE_X - 50, SCORE_Y + 50 + i * 22);
	}
	return ret;
}

// Affiche les différentes informations sur la partie droite du jeu
void Board::_drawScore() const {
	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
	SDL_Rect rectangle = { SCORE_X - 50, SCORE_Y, SCORE_WIDTH + 110, 620};
	SDL_RenderFillRect(_renderer, &rectangle);
	SDL_Color grey = { 175, 175, 175, 255 };
	SDL_Color white = { 255, 255, 255, 255 };
	_printStr(&grey, "Score", SCORE_X, SCORE_Y, TEXT_CENTER);
	char str[10];
	_itoa_s(_score, str, 10);
	_printStr(&white, str, SCORE_X, SCORE_Y + 20, TEXT_CENTER);

	_printStr(&grey, "Level", SCORE_X, SCORE_Y + 50, TEXT_CENTER);
	_itoa_s(_level, str, 10);
	_printStr(&white, str, SCORE_X, SCORE_Y + 70, TEXT_CENTER);

	_printStr(&grey, "Lines", SCORE_X, SCORE_Y + 100, TEXT_CENTER);
	_itoa_s(_lines, str, 10);
	_printStr(&white, str, SCORE_X, SCORE_Y + 120, TEXT_CENTER);

	_printStr(&grey, "High Score", SCORE_X, SCORE_Y + 150, TEXT_CENTER);
	_itoa_s(_highScore, str, 10);
	_printStr(&white, str, SCORE_X, SCORE_Y + 170, TEXT_CENTER);

	if (_nextPiece != NULL) {
		_printStr(&grey, "Next", SCORE_X, SCORE_Y + 400, TEXT_CENTER);
		_nextPiece->draw(_renderer, SCORE_X - 100, SCORE_Y + 450);
	}
}

// Affiche la grille du jeu
void Board::_draw(bool gameOver) const {
	if (gameOver)
		SDL_SetRenderDrawColor(_renderer, 22, 22, 22, 255);
	else
		SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
	SDL_Rect rectangle = { _x - BORDER_WIDTH, _y, PIECE_SIZE * GRID_WIDTH + BORDER_WIDTH * 2, PIECE_SIZE * GRID_HEIGH + BORDER_WIDTH };
	SDL_RenderFillRect(_renderer, &rectangle);
	for (int x = 1; x < GRID_WIDTH + 1; x++)
		for (int y = 0; y < GRID_HEIGH; y++)
			if (_grid[x][y] > 0)
				Piece::_drawBlock(_renderer, _grid[x][y] - '0', (x - 1) * PIECE_SIZE + _x, y * PIECE_SIZE + _y, gameOver);
	if (!gameOver && _currentPiece != NULL)
		_currentPiece->draw(_renderer, _x, _y);
}