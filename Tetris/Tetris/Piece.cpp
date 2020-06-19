#include "Piece.h"
#include <SDL.h>
#include <string.h>
#include <Windows.h>

// Tableau global des couleurs des sept pièces du jeu
SDL_Color PieceColor[] = { { 255, 0, 0, 255 },{ 255, 0, 255, 255 },{ 255, 255, 0, 255 },{ 0, 255, 255, 255 },{ 0, 0, 255, 255 },{ 211, 211, 211, 255 },{ 0, 255, 0, 255 } };

// Le constructeur initialise le tableau _display (correspond à la forme de la pièce)
// en fonction du numéro de pièce passé en paramètre
Piece::Piece(int piece)
{
	_piece = piece;
	_orientation = 0;
	_x = 4;
	_y = -1;
	memset(_display, 0, sizeof(_display));
	switch (piece) {
		case 0:
			_display[1][0] = '0';
			_display[1][1] = '0';
			_display[1][2] = '0';
			_display[1][3] = '0';
			_y++;
			break;
		case 1:
			_display[2][0] = '1';
			_display[2][1] = '1';
			_display[2][2] = '1';
			_display[1][2] = '1';
			_x--;
			_y++;
			break;
		case 2:
			_display[1][0] = '2';
			_display[1][1] = '2';
			_display[1][2] = '2';
			_display[2][2] = '2';
			_x--;
			_y++;
			break;
		case 3:
			_display[1][1] = '3';
			_display[2][1] = '3';
			_display[1][2] = '3';
			_display[2][2] = '3';
			_x--;
			break;
		case 4:
			_display[1][1] = '4';
			_display[2][1] = '4';
			_display[0][2] = '4';
			_display[1][2] = '4';
			break;
		case 5:
			_display[1][1] = '5';
			_display[0][2] = '5';
			_display[1][2] = '5';
			_display[2][2] = '5';
			break;
		case 6:
			_display[0][1] = '6';
			_display[1][1] = '6';
			_display[1][2] = '6';
			_display[2][2] = '6';
			break;
	}
}

Piece::~Piece()
{
}

// Regarde si la pièce peut être placée dans la grille du jeu (aucune pièce n'est présente pour bloquer son arrivée)
bool Piece::check(const char(*grid)[], int w, int h) const
{
	bool ret = true;
	for (int x = 0; x < 4 && ret; x++)
		for (int y = 0; y < 4; y++) {
			if (_display[x][y] > 0 && (*grid)[(_x + x + 1) * h + _y + y] > 0) {
				ret = false;
				break;
			}
		}
	return ret;
}

// Calcule la nouvelle position de la pièce lors d'une rotation
bool Piece::turn(const char(*grid)[], int w, int h)
{
	bool ret = true;
	_orientation = (_orientation + 1) % 4;
	char save[4][4];
	memcpy(save, _display, sizeof(_display));
	for (int x = 0; x < 4 && ret; x++)
		for (int y = 0; y < 4; y++) {
			_display[x][y] = save[3 - y][x];
			if (_display[x][y] > 0 && (*grid)[(_x + x + 1) * h + _y + y] > 0 || y < 0) {
				ret = false;
				memcpy(_display, save, sizeof(save));
				break;
			}
		}
	return ret;
}

// Déplace la pièce sur la gauche si aucun obstacle ne gêne
bool Piece::left(const char(*grid)[], int w, int h)
{
	byte row = 0;
	int x = 0, y = 0;
	while (row != 0x0F) {
		if ((row & (1 << y)) == 0 && _display[x][y] > 0)
			if ((*grid)[(_x + x) * h + _y + y] > 0)
				break;
			else
				row += 1 << y;
		y++;
		if (y > 3) {
			y = 0;
			x++;
			if (x > 3) {
				row = 0x0F;
				break;
			}
		}
	}
	if (row == 0x0F) {
		_x--;
		return true;
	}
	return false;
}

// Déplace la pièce sur la droite si aucun obstacle ne gêne
bool Piece::right(const char(*grid)[], int w, int h)
{
	char row = 0;
	int x = 3, y = 3;
	while (row != 0x0F) {
		if ((row & (1 << y)) == 0 && _display[x][y] > 0)
			if ((*grid)[(_x + x + 2) * h + _y + y] > 0)
				break;
			else
				row += 1 << y;
		y--;
		if (y < 0) {
			y = 3;
			x--;
			if (x < 0) {
				row = 0x0F;
				break;
			}
		}
	}
	if (row == 0x0F) {
		_x++;
		return true;
	}
	return false;
}

// Déplace la pièce vers le bas si aucun obstacle ne gêne
bool Piece::down(const char(*grid)[], int w, int h)
{
	char row = 0;
	int x = 0, y = 3;
	while (row != 0x0F) {
		if ((row & (1 << x)) == 0 && _display[x][y] > 0)
			if ((*grid)[(_x + x + 1) * h + _y + y + 1] > 0)
				break;
			else
				row += 1 << x;
		x++;
		if (x > 3) {
			x = 0;
			y--;
			if (y < 0) {
				row = 0x0F;
				break;
			}
		}
	}
	if (row == 0x0F) {
		_y++;
		return true;
	}
	return false;
}

// La pièce est bloquée, elle est ajoutée à la grille de jeu
void Piece::insert(char(*grid)[], int w, int h) {
	for (int x = 0; x < 4; x++)
		for (int y = 0; y < 4; y++) {
			if (_display[x][y] > 0)
				(*grid)[(_x + x + 1) * h + _y + y] = _display[x][y];
		}
}

int Piece::y() const {
	return _y;
}

int Piece::x() const {
	return _x;
}

// Dessine un élément d'une pièce
void Piece::_drawBlock(SDL_Renderer *rd, int piece, int x, int y, bool gameOver) {
	SDL_Rect rectangle = { 0, 0, PIECE_SIZE, PIECE_SIZE };
	SDL_Color gameOverColor = { 50, 50, 50, 255 };
	SDL_Color* pColor;
	if (gameOver)
		pColor = &gameOverColor;
	else
		pColor = &(PieceColor[piece]);
	SDL_SetRenderDrawColor(rd, pColor->r, pColor->g, pColor->b, pColor->a);
	rectangle.x = x;
	rectangle.y = y;
	SDL_RenderFillRect(rd, &rectangle);
	SDL_SetRenderDrawColor(rd, max(pColor->r, 100), max(pColor->g, 100), max(pColor->b, 100), pColor->a);
	SDL_RenderDrawLine(rd, x, y + PIECE_SIZE, x, y);
	SDL_RenderDrawLine(rd, x, y, x + PIECE_SIZE, y);
	SDL_SetRenderDrawColor(rd, pColor->r / 2, pColor->g / 2, pColor->b / 2, pColor->a);
	SDL_RenderDrawLine(rd, x + PIECE_SIZE - 1, y + PIECE_SIZE, x + PIECE_SIZE -1, y);
	SDL_RenderDrawLine(rd, x, y + PIECE_SIZE - 1, x + PIECE_SIZE, y + PIECE_SIZE - 1);
	SDL_SetRenderDrawColor(rd, 0, 0, 0, pColor->a);
	SDL_RenderDrawLine(rd, x + PIECE_SIZE, y + PIECE_SIZE, x + PIECE_SIZE, y);
	SDL_RenderDrawLine(rd, x, y + PIECE_SIZE, x + PIECE_SIZE, y + PIECE_SIZE);
}

// Dessine une pièce
void Piece::draw(SDL_Renderer *rd, int offset_x, int offset_y) {
	for (int x = 0; x < 4; x++)
		for (int y = 0; y < 4; y++)
			if (_display[x][y] > 0)
				Piece::_drawBlock(rd, _piece, _x * PIECE_SIZE + x * PIECE_SIZE + offset_x, _y * PIECE_SIZE + y * PIECE_SIZE + offset_y);
}