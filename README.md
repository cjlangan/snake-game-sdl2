# Snake Game using SDL2 Library

### Dependencies
- sdl2

## To Use
run these commands:

```bash
git clone https://github.com/cjlangan/snake-game-sdl2
make
```

Then to play the game:

```bash
./game
```

## Notes
- The amount of food, snake speed, and number of squares are customizable, however, to do so you will have to edit the `game.c` file:

In the game.c file you will find these line:

```c
#define WINDOW_WIDTH 20
#define WINDOW_HEIGHT 20
#define PIXEL_SIZE 50
#define NUM_FOOD 10
#define SPEED 800
```

NUM_FOOD is how much food will be on the screen at any given time.

SPEED ranges from 1-999, higher number being faster.  To be specific the time it takes the snake to take each step will be (1000 - SPEED) milliseconds.

The window height/width and pixel size kinda relate to eachother

For example: if you just changed PIXEL_SIZE to 200, then your window height would be 200x20=4000 pixel tall in total, which is likely too tall for your monitor
