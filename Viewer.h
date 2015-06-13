/*
 * Visor.
 */

#ifndef VIEWER_H_
#define VIEWER_H_

#include <SDL2/SDL.h>

typedef void (*Renderizador)(Uint32* pixels);

typedef struct contexto_t
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Texture* grid;
} Contexto;

typedef struct window_t
{
    char* title;
    int scale;
    int width, height;
} Window;

int init(Contexto*, Window*);

void kill(Contexto*);

void renderizar(Contexto*, Renderizador);

#endif // VIEWER_H_
