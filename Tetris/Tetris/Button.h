#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <SDL.h>

class Button
{
public:
	Button(SDL_Renderer* renderer, const char* image, int x, int y);
	~Button();
	void draw();
	bool isSelected() const;
	void select(bool isSelected);
	bool isIn(int x, int y) const;
private:
	SDL_Renderer* _renderer;
	SDL_Surface* _button;
	SDL_Texture* _jpg;
	SDL_Rect _rect;
	int _x;
	int _y;
	bool _selected;
	void _circle(int radius);
};

#endif _BUTTON_H_