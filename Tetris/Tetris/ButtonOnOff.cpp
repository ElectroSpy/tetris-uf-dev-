#include <string.h>
#include <SDL_image.h>
#include <string>
#include "ButtonOnOff.h"

using namespace std;

#define BUTTON_SIZE 100

// Gestion d'un bouton circulaire à deux positions. Utilisé dans certains menus
ButtonOnOff::ButtonOnOff(SDL_Renderer* renderer, const char* image, int x, int y)
{
	_rect = { x, y, BUTTON_SIZE, BUTTON_SIZE };
	string fileName(image);
	fileName += "On.jpg";
	_buttonOn = IMG_Load(fileName.c_str());
	_jpgOn = SDL_CreateTextureFromSurface(renderer, _buttonOn);
	fileName = image;
	fileName += "Off.jpg";
	_buttonOff = IMG_Load(fileName.c_str());
	_jpgOff = SDL_CreateTextureFromSurface(renderer, _buttonOff);
	_renderer = renderer;
	_status = true;
	_x = x;
	_y = y;
}

ButtonOnOff::~ButtonOnOff()
{
	SDL_FreeSurface(_buttonOn);
	SDL_DestroyTexture(_jpgOn);
	SDL_FreeSurface(_buttonOff);
	SDL_DestroyTexture(_jpgOff);
}

// Affiche l'image correspondant à la position du bouton
void ButtonOnOff::draw() {
	if (_status)
		SDL_RenderCopy(_renderer, _jpgOn, NULL, &_rect);
	else
		SDL_RenderCopy(_renderer, _jpgOff, NULL, &_rect);
	SDL_RenderPresent(_renderer);
}

// Retourne la position du bouton
bool ButtonOnOff::status() const {
	return _status;
}

// Inverse la position du bouton
bool ButtonOnOff::select() {
	if (_status)
		_status = false;
	else
		_status = true;
	draw();
	return _status;
}

// Retourne true si le point (x, y) est à l'intérieur du bouton
bool ButtonOnOff::isIn(int x, int y) const {
	int cx = _x + BUTTON_SIZE / 2;
	int cy = _y + BUTTON_SIZE / 2;
	if (sqrt((x - cx) * (x - cx) + (y - cy) * (y - cy)) > BUTTON_SIZE / 2)
		return false;
	return true;
}