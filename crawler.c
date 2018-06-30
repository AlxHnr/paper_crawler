#include <crawler.h>

static int crawler_count = 0;
static int adult_crawler_count = 0;

static double crawler_x = 0;
static double crawler_y = 0;
static double crawler_w = 0;
static double crawler_h = 0;

static new_crawler *first_crawler = NULL;
SDL_Surface *crawler[EVOLUTION_STEPS][DIRECTIONS];

int load_crawler(SDL_Surface *screen)
{
  SDL_Surface *cache_surface = NULL;
  SDL_Rect cache_rect;
  
  int steps;
  int directions;
  char cache_string[1024] = {0};
  
  steps = 0;
  while(steps < EVOLUTION_STEPS)
  {
    directions = 0;
    while(directions < DIRECTIONS)
    {
      /* load first direction from file */
      /* other directions are a rotatet copy from the first one */
      if(directions == 0)
      {
        sprintf(cache_string, "sprite/%i.png", steps);
        if((crawler[steps][directions] = load_image(cache_string)) == NULL)
          return -1;
      }
      else
      {
        cache_surface = rotozoomSurface(crawler[steps][0], -directions, 1, 0);
        
        /* copy crawler[0][0] first crawler and clear it, to blit cache_surface */
        crawler[steps][directions] = SDL_ConvertSurface(crawler[0][0], crawler[0][0]->format, crawler[0][0]->flags);
        SDL_FillRect(crawler[steps][directions], NULL, SDL_MapRGB(crawler[steps][directions]->format, 127, 127, 127));
        SDL_SetColorKey(crawler[steps][directions], SDL_SRCCOLORKEY, SDL_MapRGB(crawler[steps][directions]->format, 127, 127, 127));
        SDL_SetAlpha(crawler[steps][directions], 0, 0);
        
        cache_rect.x = (int)(CRAWLER_FRAME_SIZE/2 - cache_surface->w/2);
        cache_rect.y = (int)(CRAWLER_FRAME_SIZE/2 - cache_surface->h/2);
        SDL_BlitSurface(cache_surface, NULL, crawler[steps][directions], &cache_rect);
        
        SDL_FreeSurface(cache_surface);
      }
      
      /* blit status bar */
      if(directions % 4 == 0)
      {
        boxRGBA(screen, 0, screen->h - 15, screen->w, screen->h, 50, 50, 50, 255);
        sprintf(cache_string, "Loading evolution step %i/%i, direction %03i/%i", steps + 1, EVOLUTION_STEPS, directions + 1, DIRECTIONS);
        stringRGBA(screen, 0, screen->h - 10, cache_string, 255, 255, 255, 255);
        SDL_Flip(screen);
      }
      
      directions++;
    }
    
    steps++;
  }
  
  return 0;
}
void free_crawler()
{
  int steps;
  int directions;
  
  steps = 0;
  while(steps < EVOLUTION_STEPS)
  {
    directions = 0;
    while(directions < DIRECTIONS)
    {
      SDL_FreeSurface(crawler[steps][directions]);
      
      directions++;
    }
    
    steps++;
  }
}

void add_new_crawler(const double x, const double y)
{
  new_crawler *cache_crawler = NULL;
  
  if(first_crawler == NULL)
  {
    first_crawler = malloc(sizeof(struct new_crawler));
    first_crawler->next = NULL;
  }
  else
  {
    cache_crawler = first_crawler;
    first_crawler = malloc(sizeof(struct new_crawler));
    
    first_crawler->next = cache_crawler;
  }
  
  first_crawler->x = x;
  first_crawler->y = y;
  
  first_crawler->move_x = 0;
  first_crawler->move_y = 0;
  
  first_crawler->evolution_step = 0;
  first_crawler->direction = rand() % DIRECTIONS;
  
  first_crawler->life_start_time = SDL_GetTicks();
  first_crawler->last_step = SDL_GetTicks();
  
  first_crawler->speed = 1.0;
  first_crawler->rotation = 0;
  first_crawler->moving = 0;
  
  crawler_count++;
}

int count_crawler()
{
  return crawler_count;
}
int count_adult_crawler()
{
  return adult_crawler_count;
}
void set_first_crawler_pos(double *camera_x, double *camera_y)
{
  new_crawler *cache_crawler = first_crawler;
  
  if(first_crawler == NULL)
    return;
  
  while(cache_crawler->next != NULL)
    cache_crawler = cache_crawler->next;
  
  *camera_x = cache_crawler->x;
  *camera_y = cache_crawler->y;
}

double get_crawler_x()
{
  return crawler_x;
}
double get_crawler_y()
{
  return crawler_y;
}
double get_crawler_w()
{
  return crawler_w;
}
double get_crawler_h()
{
  return crawler_h;
}

void blit_crawler(SDL_Surface *screen, const int camera_x, const int camera_y)
{
  new_crawler *cache_crawler = first_crawler;
  SDL_Rect cache_dst;
  
  int cache_x, cache_y;
  
  while(cache_crawler != NULL)
  {
    /* get distance */
    cache_x = camera_x - cache_crawler->x;
    if(cache_x < 0)
      cache_x *= -1;
    
    cache_y = camera_y - cache_crawler->y;
    if(cache_y < 0)
      cache_y *= -1;
    
    if(cache_crawler->x < camera_x)
      cache_dst.x = (int)(screen->w/2 - CRAWLER_FRAME_SIZE/2) - cache_x;
    else
      cache_dst.x = (int)(screen->w/2 - CRAWLER_FRAME_SIZE/2) + cache_x;
    
    if(cache_crawler->y < camera_y)
      cache_dst.y = (int)(screen->h/2 - CRAWLER_FRAME_SIZE/2) - cache_y;
    else
      cache_dst.y = (int)(screen->h/2 - CRAWLER_FRAME_SIZE/2) + cache_y;
    
    /* check if crawler is in camera range */
    if(cache_dst.x + CRAWLER_FRAME_SIZE > 0 &&
      cache_dst.x < screen->w &&
      cache_dst.y + CRAWLER_FRAME_SIZE > 0 &&
      cache_dst.y < screen->h)
      SDL_BlitSurface(crawler[cache_crawler->evolution_step][cache_crawler->direction], NULL, screen, &cache_dst);
    
    cache_crawler = cache_crawler->next;
  }
}

void handle_crawler()
{
  new_crawler *cache_crawler = first_crawler;
  
  while(cache_crawler != NULL)
  {
    /* handle evolution */
    if(cache_crawler->evolution_step < 8 && SDL_GetTicks() - cache_crawler->life_start_time > 200000 && ((adult_crawler_count == 0) || (rand() % 30000 == 0)))
    {
      cache_crawler->evolution_step = 8;
      cache_crawler->speed = 2.0;
      
      adult_crawler_count++;
    }
    else if(cache_crawler->evolution_step < 7 && SDL_GetTicks() - cache_crawler->life_start_time > 170000)
    {
      cache_crawler->evolution_step = 7;
      cache_crawler->speed = 3.0;
    }
    else if(cache_crawler->evolution_step < 6 && SDL_GetTicks() - cache_crawler->life_start_time > 140000)
    {
      cache_crawler->evolution_step = 6;
      cache_crawler->speed = 9.0;
    }
    else if(cache_crawler->evolution_step < 5 && SDL_GetTicks() - cache_crawler->life_start_time > 120000)
    {
      cache_crawler->evolution_step = 5;
      cache_crawler->speed = 6.5;
    }
    else if(cache_crawler->evolution_step < 4 && SDL_GetTicks() - cache_crawler->life_start_time > 100000)
    {
      cache_crawler->evolution_step = 4;
      cache_crawler->speed = 5.0;
    }
    else if(cache_crawler->evolution_step < 3 && SDL_GetTicks() - cache_crawler->life_start_time > 85000)
    {
      cache_crawler->evolution_step = 3;
      cache_crawler->speed = 3.5;
    }
    else if(cache_crawler->evolution_step < 2 && SDL_GetTicks() - cache_crawler->life_start_time > 60000)
    {
      cache_crawler->evolution_step = 2;
      cache_crawler->speed = 2.0;
    }
    else if(cache_crawler->evolution_step < 1 && SDL_GetTicks() - cache_crawler->life_start_time > 40000)
    {
      cache_crawler->evolution_step = 1;
      cache_crawler->speed = 1.5;
    }
    
    /* multiply */
    if(cache_crawler->evolution_step == 8 && rand() % 6000 == 0)
      add_new_crawler(cache_crawler->x, cache_crawler->y);
    
    /* random rotation */
    if(rand() % 30 == 0)
      cache_crawler->rotation = 0;
    else if(rand() % 30 == 0)
      cache_crawler->rotation = rand() % 3 - 1;
    
    if(rand() % 250 == 0)
    {
      /* toggle movement */
      if(cache_crawler->moving)
        cache_crawler->moving = 0;
      else
        cache_crawler->moving = 1;
    }
    
    /* add direction and correct value */
    cache_crawler->direction += cache_crawler->rotation;
    if(cache_crawler->direction < 0)
      cache_crawler->direction = DIRECTIONS -1;
    else if(cache_crawler->direction == DIRECTIONS)
      cache_crawler->direction = 0;
    
    /* calculate x and y movement from direction */
    if(cache_crawler->direction >= 270)
    {
      /* up left */
      cache_crawler->move_y = cache_crawler->speed * (cache_crawler->direction - (cache_crawler->direction/90 * 90))/90;
      cache_crawler->move_x = cache_crawler->speed - cache_crawler->move_y;
      
      cache_crawler->move_x *= -1;
      cache_crawler->move_y *= -1;
    }
    else if(cache_crawler->direction >= 180)
    {
      /* down left */
      cache_crawler->move_x = cache_crawler->speed * (cache_crawler->direction - (cache_crawler->direction/90 * 90))/90;
      cache_crawler->move_y = cache_crawler->speed - cache_crawler->move_x;
      
      cache_crawler->move_x *= -1;
    }
    else if(cache_crawler->direction >= 90)
    {
      /* down right */
      cache_crawler->move_y = cache_crawler->speed * (cache_crawler->direction - (cache_crawler->direction/90 * 90))/90;
      cache_crawler->move_x = cache_crawler->speed - cache_crawler->move_y;
    }
    else
    {
      /* up right */
      cache_crawler->move_x = cache_crawler->speed * (cache_crawler->direction - (cache_crawler->direction/90 * 90))/90;
      cache_crawler->move_y = cache_crawler->speed - cache_crawler->move_x;
      
      cache_crawler->move_y *= -1;
    }
    
    /* append movement and animate */
    if(cache_crawler->moving)
    {
      cache_crawler->x += cache_crawler->move_x * ((SDL_GetTicks() - cache_crawler->last_step)/100.0);
      cache_crawler->y += cache_crawler->move_y * ((SDL_GetTicks() - cache_crawler->last_step)/100.0);
      
      /* check if crawler reached new area */
      if(cache_crawler->x < crawler_x)
        crawler_x = cache_crawler->x;
      else if(cache_crawler->x > crawler_w)
        crawler_w = cache_crawler->x;
      
      if(cache_crawler->y < crawler_y)
        crawler_y = cache_crawler->y;
      else if(cache_crawler->y > crawler_h)
        crawler_h = cache_crawler->y;
    }
    cache_crawler->last_step = SDL_GetTicks();
    
    cache_crawler = cache_crawler->next;
  }
}