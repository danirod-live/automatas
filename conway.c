// Conway

#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include "Viewer.h"

#define TAM 40
#define SCALE 10

/** Estructura de datos con el tablero que usamos durante el juego. */
static char tablero[TAM][TAM];

/**
 * Dado un tablero y una celda esta función cuenta cuántos vecinos están vivos
 * y devuelve dicho número. Las normas del juego asumen que el tablero es
 * infinito pero como no es realista asumimos que las celdas que están en los
 * bordes tienen menos vecinos.
 */
static int
contar_vecinos(char tablero[TAM][TAM], int x, int y)
{
    int cuenta = 0;
    for (int i = x - 1; i <= x + 1; i++) {
        for (int j = y - 1; j <= y + 1; j++) {
            // no me cuento a mi mismo.
            if (i == x && j == y) {
                continue;
            }
            // evita el out of bounds
            if (i < 0 || i == TAM || j < 0 || j == TAM) {
                continue;
            }
            // si tengo un vecino vivo incremento la variable.
            if (tablero[i][j]) {
                cuenta++;
            }
        }
    }
    return cuenta;
}

/**
 * Ejecuta una iteración del tablero.
 */
static void
iterar(char tablero[TAM][TAM])
{
    // Copiamos el tablero anterior porque lo necesitamos para los cálculos.
    char anterior[TAM][TAM];
    for (int y = 0; y < TAM; y++) {
        memcpy(&anterior[y], &tablero[y], TAM * sizeof(char));
    }

    // Iteración principal.
    for (int y = 0; y < TAM; y++) {
        for (int x = 0; x < TAM; x++) {
            // Si la celda estaba viva, sobrevive si tiene 2 o 3 vecinos.
            // Si la celda estaba muerta, nace si tiene 3 vecinos.
            int vecinos_vivos = contar_vecinos(anterior, x, y);
            if (anterior[x][y]) {
                tablero[x][y] = (vecinos_vivos == 2 || vecinos_vivos == 3);
            } else {
                tablero[x][y] = (vecinos_vivos == 3);
            }
        }
    }
}

/**
 * Renderiza el tablero en el array que utiliza SDL2 para renderizar.
 */
void mostrar(Uint32* pixels) {
    for (int j = 0; j < TAM; j++) {
        for (int i = 0; i < TAM; i++) {
            // Si la celda está viva blanco. Si está muerta negro.
            if (tablero[i][j]) {
                *pixels = -1;
            } else {
                *pixels = 0;
            }

            // Avanzamos.
            pixels++;
        }
    }
}

int main(int argc, char** argv)
{
    Contexto contexto;
    SDL_Event event;

    int is_active = 1;
    int last_delta = SDL_GetTicks();

    // Inicializamos el tablero.
    memset(tablero, 0, sizeof(char) * TAM * TAM);

    Window window;
    window.title = "Conway";
    window.width = window.height = TAM;
    window.scale = SCALE;
    init(&contexto, &window);

    int play = 0;
    int drawing = 0;
    int x, y;
    while (is_active) {
        while (SDL_WaitEventTimeout(&event, 10)) {
            switch (event.type) {
                case SDL_QUIT:
                    is_active = 0;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    drawing = 1;
                    x = event.button.x / SCALE;
                    y = event.button.y / SCALE;
                    tablero[x][y] = !tablero[x][y];
                    renderizar(&contexto, &mostrar);
                    break;
                case SDL_MOUSEBUTTONUP:
                    drawing = 0;
                    break;
                case SDL_MOUSEMOTION:
                    if (drawing) {
                        x = event.motion.x / SCALE;
                        y = event.motion.y / SCALE;
                        tablero[x][y] = !tablero[x][y];
                        renderizar(&contexto, &mostrar);
                    }
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                        play = !play;
                    }
            }
        }

        if (SDL_GetTicks() - last_delta > 50) {
            if (play) {
                iterar(tablero);
            }
            renderizar(&contexto, &mostrar);
            last_delta = SDL_GetTicks();
        }
    }

    kill(&contexto);
    return 0;
}
