// Hormiga de Langton.

#include <stdio.h>
#include <SDL2/SDL.h>

#include "Viewer.h"

#define NORTE 0
#define ESTE 1
#define SUR 2
#define OESTE 3

#define TAM 80
#define SCALE 8

#define IZQUIERDA 3
#define DERECHA 1

#define INFINITO

struct hormiga_t
{
    int x, y;
    int direccion;
};

struct problema_t
{
    char tablero[TAM][TAM];
    struct hormiga_t hormiga;

    int reglas;
    int* giros;
    Uint32* colores;
};

static struct problema_t problema;

void girar(struct hormiga_t* hormiga, int direccion) {
    hormiga->direccion = (hormiga->direccion + direccion) % 4;
}

void avanzar(struct hormiga_t* hormiga) {
    switch (hormiga->direccion) {
        case NORTE: hormiga->y--; break;
        case SUR: hormiga->y++; break;
        case ESTE: hormiga->x++; break;
        case OESTE: hormiga->x--; break;
    }
#ifdef INFINITO
    hormiga->x = (hormiga->x + TAM) % TAM;
    hormiga->y = (hormiga->y + TAM) % TAM;
#endif
}

void paso(struct problema_t* problema)
{
    int x = problema->hormiga.x;
    int y = problema->hormiga.y;
#ifndef INFINITO
    if (x < 0 || x == TAM || y < 0 || y >= TAM) {
        return;
    }
#endif
    char color = problema->tablero[x][y];

    problema->tablero[x][y]++;
    problema->tablero[x][y] %= problema->reglas;
    girar(&problema->hormiga, problema->giros[color]);
    avanzar(&problema->hormiga);
}

void mostrar(Uint32* pixels)
{
    for (int y = 0; y < TAM; y++) {
        for (int x = 0; x < TAM; x++) {
            char celda = problema.tablero[x][y];
            *pixels++ = problema.colores[celda];
        }
    }
}

Uint32 color_aleatorio() {
    char rojo = rand();
    char verde = rand();
    char azul = rand();

    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        return rojo << 24 | verde << 16 | azul << 8 | 0xFF;
    #else
        return 0xFF << 24 | azul << 16 | verde << 8 | rojo;
    #endif
}

void iniciar_problema(char* reglas, struct problema_t *problema)
{
    for (int x = 0; x < TAM; x++) {
        for (int y = 0; y < TAM; y++) {
            problema->tablero[x][y] = 0;
        }
    }
    problema->hormiga.x = TAM / 2;
    problema->hormiga.y = TAM / 2;
    problema->hormiga.direccion = 0;

    problema->reglas = strlen(reglas);
    problema->giros = (int *) malloc(problema->reglas * sizeof(int));
    problema->colores = (Uint32* ) malloc(problema->reglas * sizeof(Uint32));
    for (int i = 0; i < problema->reglas; i++) {
        char regla = reglas[i];
        problema->colores[i] = color_aleatorio();
        if (regla == 'I' || regla == 'i') {
            problema->giros[i] = IZQUIERDA;
        } else if (regla == 'D' || regla == 'd') {
            problema->giros[i] = DERECHA;
        } else {
            printf("No entiendo estas reglas.\n");
            exit(1);
        }
    }

    problema->colores[0] = 0;
    problema->colores[1] = -1;
}

int main(int argc, char** argv)
{
    Contexto contexto;
    Window window;
    window.title = "Hormiga de Langton";
    window.width = window.height = TAM;
    window.scale = SCALE;

    SDL_Event event;
    int is_active = 1;
    int last_delta = SDL_GetTicks();

    if (argc < 2) {
        iniciar_problema("DI", &problema);
    } else {
        iniciar_problema(argv[1], &problema);
    }

    init(&contexto, &window);

    while (is_active) {
        while (SDL_WaitEventTimeout(&event, 10)) {
            switch (event.type) {
                case SDL_QUIT:
                    is_active = 0;
                    break;
            }
        }

       if (SDL_GetTicks() - last_delta > 50) {
        for (int i = 0; i < 50; i++)
            paso(&problema);
            renderizar(&contexto, mostrar);
       }
    }

    kill(&contexto);
    return 0;
}
