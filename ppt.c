#include <SDL2/SDL.h>
#include "Viewer.h"

#define TAM 240
#define SCALE 3

#define ROJO 0
#define VERDE 1
#define AZUL 2
#define BLANCO 3

static char tablero[TAM][TAM];

void sacar_vecino(int x, int y, int* vx, int* vy) {
    int sx, sy;
    do {
        sx = x + (rand() % 3) - 1;
        sy = y + (rand() % 3) - 1;
    } while (sx < 0 || sy < 0 || sx >= TAM || sy >= TAM || (x == sx && y == sy));
    *vx = sx;
    *vy = sy;
}

void actualizar() {
    char nuevo[TAM][TAM];

    for (int y = 0; y < TAM; y++) {
        for (int x = 0; x < TAM; x++) {
            int vx, vy;
            sacar_vecino(x, y, &vx, &vy);

            char yo_color = tablero[x][y] / 10;
            char yo_inten = tablero[x][y] % 10;
            char vecino_color = tablero[vx][vy] / 10;
            char vecino_inten = tablero[vx][vy] % 10;

            if (yo_color == BLANCO) {
                if (vecino_color != BLANCO && vecino_inten < 9) {
                    yo_color = vecino_color;
                    yo_inten = vecino_inten + 1;
                }
            } else if (vecino_color != BLANCO && vecino_color != yo_color) {
                if ((yo_color == ROJO && vecino_color == VERDE) ||
                    (yo_color == VERDE && vecino_color == AZUL) ||
                    (yo_color == AZUL && vecino_color == ROJO))
                {
                    if (yo_inten > 0)
                        yo_inten--;
                    if (++vecino_inten == 10) {
                        vecino_inten = 0;
                        vecino_color = yo_color;
                    }
                } else {
                    if (vecino_inten > 0)
                        vecino_inten--;
                    if (++yo_inten == 10) {
                        yo_inten = 0;
                        yo_color = vecino_color;
                    }
                }
            }

            nuevo[x][y] = yo_color * 10 + yo_inten;
            nuevo[vx][vy] = vecino_color * 10 + vecino_inten;
        }
    }

    for (int y = 0; y < TAM; y++) {
        memcpy(&tablero[y], &nuevo[y], sizeof(char) * TAM);
    }
}

void dibujar(Uint32* pixels) {
    for (int y = 0; y < TAM; y++) {
        for (int x = 0; x < TAM; x++) {
            switch (tablero[x][y] / 10) {
                case ROJO:  *pixels = 0xFF0000FF; break;
                case VERDE: *pixels = 0x00FF00FF; break;
                case AZUL:  *pixels = 0x0000FFFF; break;
                case BLANCO:*pixels = -1;
            }
            pixels++;
        }
    }
}

int main() {
    Contexto contexto;
    Window window;
    window.title = "Piedra papel tijera";
    window.width = window.height = TAM;
    window.scale = SCALE;
    init(&contexto, &window);

    memset(tablero, BLANCO * 10, sizeof(char) * TAM * TAM);

    SDL_Event ev;
    int last_delta = SDL_GetTicks();
    int show_must_go_on = 1;

    int brocha = BLANCO;
    int drawing = 0;
    int pausa = 1;

    int x, y;
    while (show_must_go_on) {
        while (SDL_WaitEventTimeout(&ev, 10)) {
            switch (ev.type) {
                case SDL_QUIT:
                    show_must_go_on = 0;
                    break;
                case SDL_KEYDOWN:
                    if (ev.key.keysym.scancode == SDL_SCANCODE_1) {
                        brocha = ROJO;
                    } else if (ev.key.keysym.scancode == SDL_SCANCODE_2) {
                        brocha = VERDE;
                    } else if (ev.key.keysym.scancode == SDL_SCANCODE_3) {
                        brocha = AZUL;
                    } else if (ev.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                        pausa = !pausa;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    drawing = 1;
                    x = ev.button.x / SCALE;
                    y = ev.button.y / SCALE;
                    tablero[x][y] = brocha * 10;
                    renderizar(&contexto, &dibujar);
                    break;
                case SDL_MOUSEBUTTONUP:
                    drawing = 0;
                    break;
                case SDL_MOUSEMOTION:
                    if (drawing) {
                        x = ev.motion.x / SCALE;
                        y = ev.motion.y / SCALE;
                        tablero[x][y] = brocha * 10;
                        if (!pausa) {
                            actualizar();
                        }
                        renderizar(&contexto, &dibujar);
                    }
            }
        }

        if (SDL_GetTicks() - last_delta > 10) {
            if (!pausa) {
                actualizar();
            }
            renderizar(&contexto, &dibujar);
            last_delta = SDL_GetTicks();
        }
    }

    kill(&contexto);
}
