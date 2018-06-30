#ifndef CRAWLER_H
#define CRAWLER_H

#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_rotozoom.h>
#include <math.h>

#include <graphic.h>

#define DIRECTIONS 360
#define EVOLUTION_STEPS 9
#define CRAWLER_FRAME_SIZE 96
#define FRAME_DURATION 100

typedef struct new_crawler{
  double x, y;
  double move_x, move_y;
  
  int evolution_step, direction;
  double speed;
  int rotation;
  int moving;
  
  Uint32 life_start_time;
  Uint32 last_step;
  
  struct new_crawler *next;
}new_crawler;

int load_crawler(SDL_Surface *screen);
void free_crawler();

void add_new_crawler(const double x, const double y);

int count_crawler();
int count_adult_crawler();
void set_first_crawler_pos(double *camera_x, double *camera_y);

double get_crawler_x();
double get_crawler_y();
double get_crawler_w();
double get_crawler_h();

void blit_crawler(SDL_Surface *screen, const int camera_x, const int camera_y);

void handle_crawler();

#endif