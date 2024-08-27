#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define WINDOW_WIDTH 20
#define WINDOW_HEIGHT 20
#define PIXEL_SIZE 50
#define NUM_FOOD 10
#define SPEED 800

#define UP 'u'
#define DOWN 'd'
#define LEFT 'l'
#define RIGHT 'r'

typedef struct NODE node;
struct NODE
{
    node *link;
    int x;
    int y;
};

typedef struct SNAKE snake;
struct SNAKE
{
    node *head;
    node *back;
    int size;
};

snake *make_snake(void);
void enqueue(snake *s, int x, int y);
void dequeue(snake *s, int *x, int *y);
void destroy_snake(snake *s);

bool isCollision(SDL_Rect *player, snake *s, int width, int height);

void moveFood(snake *s, SDL_Rect *player, SDL_Rect food[], int idx);

bool onFood(SDL_Rect *player, SDL_Rect food[], int *foodIdx);

// Retrieve key presses from the user
bool getInput(SDL_Event *event, char *direction);

// Based on Key presses, move the player
void movePlayer(SDL_Rect *player, char direction, int distance);

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Snake", 
                        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                        WINDOW_WIDTH * PIXEL_SIZE, WINDOW_HEIGHT * PIXEL_SIZE, 
                        SDL_WINDOW_SHOWN);
    SDL_Surface *screenSurface = SDL_GetWindowSurface(window);
    
    // Create the player (a square)
    SDL_Rect player;
    player.x = WINDOW_WIDTH / 2 * PIXEL_SIZE;
    player.y = WINDOW_HEIGHT / 2 * PIXEL_SIZE;
    player.w = PIXEL_SIZE;
    player.h = PIXEL_SIZE;

    // Create Food on Map
    SDL_Rect food[NUM_FOOD];
    srand(time(NULL));
    for(int i = 0; i < NUM_FOOD; i++)
    {
        food[i].x = rand() % WINDOW_WIDTH * PIXEL_SIZE;
        food[i].y = rand() % WINDOW_HEIGHT * PIXEL_SIZE;
        food[i].w = PIXEL_SIZE;
        food[i].h = PIXEL_SIZE;
    }
    
    snake *s = make_snake();
    enqueue(s, player.x, player.y); // Starting position

    SDL_Event event;
    bool gameOn = true;
    char direction = UP;
    int x, y, foodIdx;

    while(gameOn)
    {
        SDL_Delay(1000 - SPEED);
        gameOn = getInput(&event, &direction);
        movePlayer(&player, direction, PIXEL_SIZE);
        
        if(!isCollision(&player, s, WINDOW_WIDTH * PIXEL_SIZE, 
                                    WINDOW_HEIGHT * PIXEL_SIZE))
        {

            if(!onFood(&player, food, &foodIdx))
            {
                dequeue(s, &x, &y); // Regular movement (no food)
            }
            else
            {
                moveFood(s, &player, food, foodIdx);
            }
            enqueue(s, player.x, player.y);


            // Clear the screen with a black colour
            SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 
                                                            0x00, 0x00, 0x00));
            
            // Render The Food
            for(int i = 0; i < NUM_FOOD; i++)
            {
                SDL_FillRect(screenSurface, &food[i], SDL_MapRGB(screenSurface->format,
                                                                0xFF, 0x00, 0x00));
            }

            // Iterate through stack of nodes to render the snake
            node *curr = s->head;
            while(curr != NULL)
            {
                player.x = curr->x;
                player.y = curr->y;
                SDL_FillRect(screenSurface, &player, SDL_MapRGB(screenSurface->format,
                                                                0xFF, 0xFF, 0xFF));
                curr = curr->link;
            }

            SDL_UpdateWindowSurface(window);

            if(s->size == WINDOW_WIDTH * WINDOW_HEIGHT)
            {
                gameOn = false;
                printf("You Win!\n");
            }
        }
        else
        {
            gameOn = false;
            printf("You Lose.\n");
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    destroy_snake(s);

    return EXIT_SUCCESS;
}

bool isCollision(SDL_Rect *player, snake *s, int width, int height)
{
    bool isCollision = false;
    
    if(player->x < 0 || player->y < 0 || player->x >= width || player->y >= height)
    {
        isCollision = true;
    }
    else
    {
        node *curr = s->head;
        while(curr != NULL && !isCollision)
        {
            if(curr->x == player->x && curr->y == player->y)
            {
                isCollision = true;
            }
            curr = curr->link;
        }
    }
    return isCollision;
}

void moveFood(snake *s, SDL_Rect *player, SDL_Rect food[], int idx)
{
    bool isCollision;

    do
    {
        food[idx].x = rand() % WINDOW_WIDTH * PIXEL_SIZE;
        food[idx].y = rand() % WINDOW_HEIGHT * PIXEL_SIZE;

        isCollision = false;

        for(int i = 0; i < NUM_FOOD && !isCollision; i++)
        {
            if(i != idx && food[i].x == food[idx].x && food[i].y == food[idx].y)
            {
                isCollision = true;
            }
        }

        node *curr = s->head;
        while(curr != NULL && !isCollision)
        {
            if(curr->x == food[idx].x && curr->y == food[idx].y)
            {
                isCollision = true;
            }
            curr = curr->link;
        }

    }while(isCollision);
}

bool onFood(SDL_Rect *player, SDL_Rect food[], int *foodIdx)
{
    bool onFood = false;
    int idx = 0;

    while(!onFood && idx < NUM_FOOD)
    {
        if(food[idx].x == player->x && food[idx].y == player->y)
        {
            *foodIdx = idx;
            onFood = true;
        }
        idx++;
    }

    return onFood;
}

// Return whether the game should terminate, also takes in keyboard input.
bool getInput(SDL_Event *event, char *direction)
{
    bool continueGame = true;

    while(SDL_PollEvent(event) != 0)
    {
        // Handling Key Presses
        if(event->type == SDL_KEYDOWN)
        {
            switch(event->key.keysym.sym)
            {
                case 'q':
                    continueGame = false;
                    break;
                case SDLK_UP:
                    if(*direction != DOWN)
                    {
                        *direction = UP;
                    }
                    break;
                case SDLK_DOWN:
                    if(*direction != UP)
                    {
                        *direction = DOWN;
                    }
                    break;
                case SDLK_LEFT:
                    if(*direction != RIGHT)
                    {
                        *direction = LEFT;
                    }
                    break;
                case SDLK_RIGHT:
                    if(*direction != LEFT)
                    {
                        *direction = RIGHT;
                    }
                    break;
            }
        }
    }

    return continueGame;
}

void movePlayer(SDL_Rect *player, char direction, int distance)
{
    switch(direction)
        {
            case UP:
                player->y -= distance;
                break;
            case DOWN:
                player->y += distance;
                break;
            case LEFT:
                player->x -= distance;
                break;
            case RIGHT:
                player->x += distance;
                break;
        }
}


snake *make_snake(void)
{
    snake *s = NULL;
    s = malloc(sizeof(snake));

    if(s != NULL)
    {
        s->head = NULL;
        s->back = NULL;
        s->size = 0;
    }
    return s;
}

void enqueue(snake *s, int x, int y)
{
    node *new_node = malloc(sizeof(node));

    if(s != NULL && new_node != NULL)
    {
        new_node->x = x;
        new_node->y = y;
        new_node->link = NULL;
            
        if(s->back != NULL)
        {
            s->back->link = new_node;
        }
        else
        {
            s->head = new_node;
        }
        
        s->back = new_node;
        s->size++;
    }
}

void dequeue(snake *s, int *x, int *y)
{
    if(s != NULL && s->head != NULL && x != NULL && y != NULL)
    {
        *x = s->head->x;
        *y = s->head->y;
        node *temp = s->head;
        s->head = s->head->link;

        if(s->head == NULL)
        {
            s->back = NULL;
        }

        free(temp);
        s->size--;
    }
}

void destroy_snake(snake *s)
{
    node *curr, *prev;

    if(s != NULL)
    {
        curr = s->head;

        while(curr != NULL)
        {
            prev = curr;
            curr = curr->link;
            free(prev);
        }

        free(s);
        s = NULL;
    }
}
