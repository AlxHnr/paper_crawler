#include <tile.h>

SDL_Surface *tile = NULL;

int load_tile()
{
  if((tile = load_image("tile.jpg")) == NULL)
    return -1;
  
  return 0;
}
void free_tile()
{
  SDL_FreeSurface(tile);
}

void blit_tile(SDL_Surface *screen, const int x, const int y)
{
  SDL_Rect cache_dest = {-tile->w, -tile->h, 0, 0};
  SDL_Rect cache_dest_2;
  
  while(cache_dest.y <= screen->h)
  {
    while(cache_dest.x <= screen->w + tile->w)
    {
      cache_dest_2 = cache_dest;
      cache_dest_2.x -= x%tile->w;
      cache_dest_2.y -= y%tile->h;
      
      SDL_BlitSurface(tile, NULL, screen, &cache_dest_2);
      
      cache_dest.x += tile->w;
    }
    
    cache_dest.x = -tile->w;
    cache_dest.y += tile->h;
  }
}