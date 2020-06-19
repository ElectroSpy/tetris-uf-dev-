#ifndef _PIECE_H_
#define _PIECE_H_
#include <SDL.h>

#define PIECE_SIZE 35
#define PIECE_I 0
#define PIECE_J 1
#define PIECE_L 2
#define PIECE_O 3
#define PIECE_S 4
#define PIECE_T 5
#define PIECE_Z 6

class Piece
{
public:
	Piece(int piece);
	~Piece();
	bool check(const char(*grid)[], int w, int h) const;
	bool turn(const char(*grid)[], int w, int h);
	bool left(const char(*grid)[], int w, int h);
	bool right(const char(*grid)[], int w, int h);
	bool down(const char(*grid)[], int w, int h);
	void insert(char(*grid)[], int w, int h);
	void draw(SDL_Renderer *rd, int offset_x, int offset_y);
	int y() const;
	int x() const;
	static void _drawBlock(SDL_Renderer *rd, int piece, int x, int y, bool gameOver = false);
	
private:
	int _piece;
	int _orientation;
	int _x;
	int _y;
	char _display[4][4];
};
#endif _PIECE_H_