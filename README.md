![preview.png](https://raw.github.com/AlxHnr/paper_crawler/master/preview.png)

I created the drawings on paper and used a scanner to digitalize them. Move
the mouse to the borders of the window to scroll trough the world. Press 1
to focus on crawler number one. Press space to focus on the center of the
world.

**Note**: This simulation is single-threaded and it can take up to 50
minutes to get to a reasonable population.

## Building

This program requires the development files of the following libraries:

* SDL (_not_ SDL2)
* SDL\_image
* SDL\_gfx

Build and run the simulation using only one thread: `make -j1 run`.
