#include "Viewer.h"
#include <stdio.h>

static SDL_Window*
create_window(Window* window)
{
    return SDL_CreateWindow(window->title,
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            window->width * window->scale, window->height * window->scale,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
}

static void
render_grid(SDL_Texture* texture, int ancho, int alto, int tam)
{
    void* pixels;
    int pitch;
    SDL_LockTexture(texture, NULL, &pixels, &pitch);
    Uint32* pos = (Uint32 *) pixels;
    for (int y = 0; y < ancho; y++) {
        for (int x = 0; x < alto; x++) {
            if (y % tam == 0 || x % tam == 0) {
                *pos = 0x80808080;
            } else {
                *pos = 0;
            }
            pos++;
        }
    }
    SDL_UnlockTexture(texture);
}

static SDL_Texture*
build_grid(Contexto* contexto, Window* window)
{
    int ancho_ventana = window->width * window->scale;
    int alto_ventana = window->height * window->scale;
    int ancho_cuadro = window->scale;

    // Construye una textura.
    SDL_Texture* grid = SDL_CreateTexture(contexto->renderer,
            SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
            window->width * window->scale, window->height * window->scale);

    if (grid != NULL) {
        render_grid(grid, ancho_ventana, alto_ventana, ancho_cuadro);
    }

    return grid;
}

int
init(Contexto* contexto, Window* window)
{
    // Intenta inicializar SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    // Intenta inicializar la ventana
    contexto->window = create_window(window);
    if (contexto->window == NULL) {
        fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    // Intenta inicializar el renderer
    contexto->renderer = SDL_CreateRenderer(contexto->window,
            -1, SDL_RENDERER_ACCELERATED);
    if (contexto->renderer == NULL) {
        fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(contexto->window);
        return 1;
    }

    // Intenta inicializar la textura
    contexto->texture = SDL_CreateTexture(contexto->renderer,
            SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
            window->width, window->height);
    if (contexto->texture == NULL) {
        fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(contexto->renderer);
        SDL_DestroyWindow(contexto->window);
        return 1;
    }

    if (window->scale > 1) {
        contexto->grid = build_grid(contexto, window);
    } else {
        contexto->grid = NULL;
    }

    SDL_SetRenderDrawBlendMode(contexto->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(contexto->grid, SDL_BLENDMODE_BLEND);

    return 0;
}

void
kill(Contexto* contexto)
{
    SDL_DestroyTexture(contexto->texture);
    SDL_DestroyRenderer(contexto->renderer);
    SDL_DestroyWindow(contexto->window);
    SDL_Quit();
}

void
renderizar(Contexto* contexto, Renderizador renderer)
{
    void* pixels;
    int pitch;

    // Renderizamos el tablero sobre la textura
    SDL_LockTexture(contexto->texture, NULL, &pixels, &pitch);
    renderer((Uint32 *) pixels);
    SDL_UnlockTexture(contexto->texture);

    // La textura va a la pantalla
    SDL_RenderClear(contexto->renderer);
    SDL_RenderCopy(contexto->renderer, contexto->texture, NULL, NULL);
    if (contexto->grid != NULL) {
        SDL_RenderCopy(contexto->renderer, contexto->grid, NULL, NULL);
    }
    SDL_RenderPresent(contexto->renderer);
}
