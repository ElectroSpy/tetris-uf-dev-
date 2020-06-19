#include <string.h>
#include <SDL_image.h>
#include "Button.h"

#define BUTTON_SIZE 100

// Gestion d'un bouton circulaire contenant une image. Utilisé dans les menus
Button::Button(SDL_Renderer* renderer, const char* image, int x, int y)
{
	_rect = { x, y, BUTTON_SIZE, BUTTON_SIZE };
	_button = IMG_Load(image);
	_jpg = SDL_CreateTextureFromSurface(renderer, _button);
	_renderer = renderer;
	_selected = false;
	_x = x;
	_y = y;
}

Button::~Button()
{
	SDL_FreeSurface(_button);
	SDL_DestroyTexture(_jpg);
}

// Dessine le bouton
void Button::draw() {
	SDL_RenderCopy(_renderer, _jpg, NULL, &_rect);
}

// Retourne true si le bouton est sélectionné
bool Button::isSelected() const {
	return _selected;
}

// Sélection/déselection d'un bouton
void Button::select(bool isSelected) {
	_selected = isSelected;
	int color = 0;
	if (_selected)
		color = 200;
	SDL_SetRenderDrawColor(_renderer, color, color, color, 255);
	_circle(BUTTON_SIZE / 2 + 1);
	_circle(BUTTON_SIZE / 2);
	_circle(BUTTON_SIZE / 2 - 1);
}

// Retourne true si le point (x, y) est à l'intérieur du bouton
bool Button::isIn(int x, int y) const {
	int cx = _x + BUTTON_SIZE / 2;
	int cy = _y + BUTTON_SIZE / 2;
	if (sqrt((x - cx) * (x - cx) + (y - cy) * (y - cy)) > BUTTON_SIZE / 2)
		return false;
	return true;
}

// Dessine un cercle de rayon radius. Utilisé pour montrer le bouton sélectionné
void Button::_circle(int radius)
{
	int d, y, x;
	int cx = _x + BUTTON_SIZE / 2;
	int cy = _y + BUTTON_SIZE / 2;
	d = 3 - (2 * radius);
	x = 0;
	y = radius;
	while (y >= x) {
		SDL_RenderDrawPoint(_renderer, cx + x, cy + y);
		SDL_RenderDrawPoint(_renderer, cx + y, cy + x);
		SDL_RenderDrawPoint(_renderer, cx - x, cy + y);
		SDL_RenderDrawPoint(_renderer, cx - y, cy + x);
		SDL_RenderDrawPoint(_renderer, cx + x, cy - y);
		SDL_RenderDrawPoint(_renderer, cx + y, cy - x);
		SDL_RenderDrawPoint(_renderer, cx - x, cy - y);
		SDL_RenderDrawPoint(_renderer, cx - y, cy - x);
		if (d < 0)
			d = d + (4 * x) + 6;
		else {
			d = d + 4 * (x - y) + 10;
			y--;
		}
		x++;
	}
}