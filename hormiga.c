// Hormiga de Langton.

#include <stdio.h>
#include <SDL2/SDL.h>

#define NORTE 0
#define ESTE 1
#define SUR 2
#define OESTE 3

#define TAM 256

#define IZQUIERDA 3
#define DERECHA 1

struct hormiga_t
{
    int x, y;
    int direccion;
};

struct problema_t
{
    char tablero[TAM][TAM];
    struct hormiga_t hormiga;
};

void girar(struct hormiga_t* hormiga, int direccion) {
    hormiga->direccion = (hormiga->direccion + direccion) % 4;
}

void avanzar(struct hormiga_t* hormiga) {
    switch (hormiga->direccion) {
        case NORTE: hormiga->y--; return;
        case SUR: hormiga->y++; return;
        case ESTE: hormiga->x++; return;
        case OESTE: hormiga->x--; return;
    }
}

void paso(struct problema_t* problema)
{
    int x = problema->hormiga.x;
    int y = problema->hormiga.y;
    char color = problema->tablero[x][y];
    switch (color) {
        case 0: // NEGRO
            problema->tablero[x][y] = 1;
            girar(&problema->hormiga, IZQUIERDA);
            break;
        case 1: // BLANCO
            problema->tablero[x][y] = 0;
            girar(&problema->hormiga, DERECHA);
            break;
    }
    avanzar(&problema->hormiga);
}

void mostrar(char tablero[TAM][TAM], Uint32* pixels)
{
    for (int y = 0; y < TAM; y++) {
        for (int x = 0; x < TAM; x++) {
            char celda = tablero[x][y];
            if (celda == 0) {
                *pixels = 0;
            } else {
                *pixels = -1;
            }
            pixels++;
        }
    }
}

int main(int argc, char** argv)
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    SDL_Event event;
    int is_active = 1;
    int debe_mostrar = 1;
    int last_delta = SDL_GetTicks();
    void* pixels;
    int pitch;

    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow("Hormiga", SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, 2 * TAM, 2 * TAM, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING, TAM, TAM);

    struct problema_t problema;
    for (int x = 0; x < TAM; x++) {
        for (int y = 0; y < TAM; y++) {
            problema.tablero[x][y] = 0;
        }
    }
    problema.hormiga.x = 100;
    problema.hormiga.y = 100;

    while (is_active) {
        while (SDL_WaitEventTimeout(&event, 10)) {
            switch (event.type) {
                case SDL_QUIT:
                    is_active = 0;
                    break;
            }
        }

       // if (debe_mostrar) {
            for (int i = 0; i < 50; i++)
            paso(&problema);

            SDL_LockTexture(texture, NULL, &pixels, &pitch);
            mostrar(problema.tablero, (Uint32*) pixels);
            SDL_UnlockTexture(texture);

            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
            //debe_mostrar = 0;
       // }
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}
