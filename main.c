/* Compile:
 * paru -Syu sdl2_ttf sdl2
 * gcc -o catamole main.c -lSDL2 -lSDL2_ttf -lm -g
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <stdint.h>
#include <time.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define STARTING_DIFFICULTY 5

SDL_Color white = {255, 255, 255, 0};
TTF_Font *font;

void create_text(SDL_Renderer *renderer, int x, int y, char *text, SDL_Texture **texture, SDL_Rect *rect) {
    SDL_Surface *message_surface = TTF_RenderText_Solid(font, text, white);
    *texture = SDL_CreateTextureFromSurface(renderer, message_surface);
    rect->w = message_surface->w;
    rect->h = message_surface->h;
    rect->x = x;
    rect->y = y;
    SDL_FreeSurface(message_surface);
}

int main(int argc, char **argv) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL initalization failed: `%s'\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window *window = SDL_CreateWindow("Catamohle", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH,
                                          WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        SDL_Log("SDL creating window failed: `%s'\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        SDL_Log("SDL creating renderer failed: `%s'\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    TTF_Init();
    font = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSans.ttf", 24);
    if (font == NULL) {
        SDL_Log("SDL failed to open font: `%s'\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Rect message_rect;
    SDL_Rect cat_rect;
    cat_rect.w = cat_rect.h = 100;
    cat_rect.x = rand() % (WINDOW_WIDTH - 100);
    cat_rect.y = rand() % (WINDOW_HEIGHT - 100);
    uint32_t score = 0;
    struct timespec now, last_rect_time;
    clock_gettime(CLOCK_MONOTONIC_RAW, &last_rect_time);
    uint8_t hit = 0;
    uint8_t lives = 3;
    double time_left_sec = STARTING_DIFFICULTY;

    while (1) {
        SDL_Event event;
        SDL_PollEvent(&event);
        switch (event.type) {
        case SDL_QUIT:
            goto exit;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
                goto exit;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                if (x >= cat_rect.x && x <= cat_rect.x + 100 && y >= cat_rect.y && y <= cat_rect.y + 100) {
                    hit = 1;
                }
            }
            break;
        }

        // update game state
        clock_gettime(CLOCK_MONOTONIC_RAW, &now);
        if (now.tv_sec - last_rect_time.tv_sec > time_left_sec) {
            SDL_Log("life lost");
            --lives;
            if (lives == 0) goto exit;
            last_rect_time = now;
            cat_rect.x = rand() % (WINDOW_WIDTH - 100);
            cat_rect.y = rand() % (WINDOW_HEIGHT - 100);
        } else if (hit) {
            SDL_Log("hit");
            hit = 0;
            ++score;
            if (score % 5 == 0) {
                time_left_sec = STARTING_DIFFICULTY - pow((double)score, 0.3);
            }
            last_rect_time = now;
            cat_rect.x = rand() % (WINDOW_WIDTH - 100);
            cat_rect.y = rand() % (WINDOW_HEIGHT - 100);
        }

        // draw background
        SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
        SDL_RenderClear(renderer);

        // draw score
        char text[33];
        snprintf(text, 33, "Score: %d, Lives: %d, Time: %.2f", score, lives, time_left_sec);
        SDL_Texture *message_texture;
        create_text(renderer, 100, 100, text, &message_texture, &message_rect);
        SDL_RenderCopy(renderer, message_texture, 0, &message_rect);
        SDL_DestroyTexture(message_texture);

        // draw cat
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &cat_rect);

        SDL_RenderPresent(renderer);
    }

exit:
    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
