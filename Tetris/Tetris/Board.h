#ifndef _BOARD_H_
#define _BOARD_H_

#include <SDL_ttf.h>
#include <fmod.h>
#include "Piece.h"
#include "Top10.h"

#define GRID_WIDTH 10
#define GRID_HEIGH 22
#define BORDER_WIDTH 3
#define TEXT_CENTER -1
#define TEXT_LEFT -2
#define TEXT_RIGHT -3
#define SCORE_WIDTH 150
#define SCORE_X 525
#define SCORE_Y 50

class Board
{
public:
	Board(SDL_Window* window, SDL_Renderer* rd, int highScore);
	~Board();
	void init();
	int start();
	int drawTop10(const Top10* top10, int index, int score) const;
private:
	Piece *_currentPiece;
	Piece *_nextPiece;
	char _grid[GRID_WIDTH + 2][GRID_HEIGH + 1];
	SDL_Window* _window;
	SDL_Renderer* _renderer;
	TTF_Font* _pFont;
	int _score;
	int _level;
	int _lines;
	int _x;
	int _y;
	int _width;
	int _height;
	int _highScore;
	FMOD_SYSTEM *_sndSystem;
	FMOD_SOUND *_sndFullLine;
	FMOD_SOUND *_sndDropDown;
	FMOD_SOUND *_sndGameOver;
	FMOD_SOUND *_sndThunder;
	void _draw(bool gameOver = false) const;
	void _drawScore() const;
	void _printStr(SDL_Color* pColor, const char *text, int x, int y, int position = 0) const;
};

#endif _BOARD_H_