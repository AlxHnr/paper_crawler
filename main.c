#include <stdlib.h>
#include <SDL.h>
#include <SDL_gfxPrimitives.h>

#include <graphic.h>
#include <framerate.h>
#include <crawler.h>
#include <tile.h>

#define MOVE_MAP_ZONE 45
#define MOVE_MAP_STEP 20.0

#ifdef __WIN32__
  int WinMain(int argc, char *argv[])
#else
  int main(int argc, char *argv[])
#endif
{
  /* variables */
  SDL_Surface *screen = NULL;
  SDL_Event user_input;
  
  SDL_Surface *cursor = NULL;
  SDL_Rect cursor_dest;
  
  Uint32 time_stop;
  
  Uint32 last_camera_move = 0;
  double camera_x = 0;
  double camera_y = 0;
  
  char cache_string[1024] = {0};
  
  /* init SDL start */
  if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    exit(EXIT_FAILURE);
  atexit(SDL_Quit);
  
  if((screen = SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE)) == NULL)
    exit(EXIT_FAILURE);
  
  SDL_WM_SetCaption("Paper Crawler - Simulation", NULL);
  SDL_ShowCursor(SDL_DISABLE);
  /* init SDL end */
  
  /* load other stuff */
  if(load_crawler(screen) == -1)
    return EXIT_FAILURE;
  atexit(free_crawler);
  add_new_crawler(0, 0);
  
  if(load_tile() == -1)
    return EXIT_FAILURE;
  atexit(free_tile);
  
  if((cursor = load_image("cursor.png")) == NULL)
    exit(EXIT_FAILURE);
  
  /* main loop start */
  time_stop = SDL_GetTicks();
  while(user_input.type != SDL_QUIT)
  {
    /* handle user_input */
    while(SDL_PollEvent(&user_input))
    {
      if(user_input.type == SDL_KEYDOWN)
      {
        if(user_input.key.keysym.sym == SDLK_SPACE)
        {
          camera_x = 0;
          camera_y = 0;
        }
        else if(user_input.key.keysym.sym == SDLK_1)
        {
          set_first_crawler_pos(&camera_x, &camera_y);
        }
      }
    }
    
    /* handle mouse events */
    if(user_input.type == SDL_MOUSEMOTION)
    {
      /* move camera */
      if(user_input.motion.x > 0 && user_input.motion.x < MOVE_MAP_ZONE)
        camera_x -= MOVE_MAP_STEP * ((float)(SDL_GetTicks() - last_camera_move)/100.0);
      else if(user_input.motion.x > screen->w - MOVE_MAP_ZONE && user_input.motion.x < screen->w)
        camera_x += MOVE_MAP_STEP * ((float)(SDL_GetTicks() - last_camera_move)/100.0);
      
      if(user_input.motion.y > 0 && user_input.motion.y < MOVE_MAP_ZONE)
        camera_y -= MOVE_MAP_STEP * ((float)(SDL_GetTicks() - last_camera_move)/100.0);
      else if(user_input.motion.y > screen->h - MOVE_MAP_ZONE && user_input.motion.y < screen->h)
        camera_y += MOVE_MAP_STEP * ((float)(SDL_GetTicks() - last_camera_move)/100.0);
      
      /* set cursor */
      cursor_dest.x = user_input.motion.x;
      cursor_dest.y = user_input.motion.y;
    }
    last_camera_move = SDL_GetTicks();
    
    /* handle crawlers */
    handle_crawler();
    
    /* blit background */
    blit_tile(screen, camera_x, camera_y);
    
    /* blit crawlers */
    blit_crawler(screen, camera_x, camera_y);
    
    /* blit status bar */
    boxRGBA(screen, 0, screen->h - 15, screen->w, screen->h, 180, 180, 180, 180);
    sprintf(cache_string, "Pos: %i/%i, Time: %02i:%02i:%02i, Population: %i, Adults: %i, Explored Surface: %llu",
          (int)(camera_x), (int)(camera_y),
          (int)((SDL_GetTicks() - time_stop)/3600000),
          (int)((SDL_GetTicks() - time_stop)/60000 - ((SDL_GetTicks() - time_stop)/3600000) * 60),
          (int)((SDL_GetTicks() - time_stop)/1000 - ((SDL_GetTicks() - time_stop)/60000) * 60),
          count_crawler(), count_adult_crawler(),
          (long long int)((get_crawler_w() - get_crawler_x()) * (get_crawler_h() - get_crawler_y())/1000));
    stringRGBA(screen, 0, screen->h - 10, cache_string, 0, 0, 0, 180);
    
    SDL_BlitSurface(cursor, NULL, screen, &cursor_dest);
    
    /* show screen */
    SDL_Flip(screen);
    controll_framerate();
  }
  /* main loop end */
  
  SDL_FreeSurface(cursor);
  return EXIT_SUCCESS;
}