#ifndef _BUTTONONOFF_H_
#define _BUTTONONOFF_H_

#include <SDL.h>

class ButtonOnOff
{
public:
	ButtonOnOff(SDL_Renderer* renderer, const char* image, int x, int y);
	~ButtonOnOff();
	void draw();
	bool status() const;
	bool select();
	bool isIn(int x, int y) const;
private:
	SDL_Renderer* _renderer;
	SDL_Surface* _buttonOn;
	SDL_Texture* _jpgOn;
	SDL_Surface* _buttonOff;
	SDL_Texture* _jpgOff;
	SDL_Rect _rect;
	int _x;
	int _y;
	bool _status;
};

#endif _BUTTONONOFF_H_