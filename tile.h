#ifndef TILE_H
#define TILE_H

#include <SDL.h>
#include <graphic.h>

int load_tile();
void free_tile();

void blit_tile(SDL_Surface *screen, const int x, const int y);

#endif