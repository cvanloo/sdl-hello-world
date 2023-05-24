/* Compile:
 * gcc -o catamhole main.c `sdl2-config --cflags --libs` -lSDL2_ttf -g
 */

#include <stdint.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL_ttf.h>

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL initalization failed: `%s'\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window *window = SDL_CreateWindow("Catamohle", SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    if (window == 0) {
        SDL_Log("SDL creating window failed: `%s'\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == 0) {
        SDL_Log("SDL creating renderer failed: `%s'\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    // ------
    // Setup font
    // ------
    TTF_Init();
    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/open-sans/OpenSans-Regular.ttf", 24);
    if (font == 0) {
        SDL_Log("SDL failed to open font: `%s'\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    SDL_Color white = {255, 255, 255};
    SDL_Surface *message_surface = TTF_RenderText_Solid(font, "Hello, SDL!", white);
    SDL_Texture *message_texture = SDL_CreateTextureFromSurface(renderer, message_surface);
    SDL_Rect message_rect;
    message_rect.x = 10;
    message_rect.y = 10;
    message_rect.w = 500;
    message_rect.h = 100;
    SDL_RenderCopy(renderer, message_texture, 0, &message_rect);
    SDL_RenderPresent(renderer);

    SDL_Rect rect;
    uint32_t score = 0;
    struct timespec now, last_rect;
    clock_gettime(CLOCK_MONOTONIC_RAW, &last_rect);
    uint8_t hit = 0;
    while (1) {
        SDL_Event event;
        SDL_PollEvent(&event);
        switch (event.type) {
        case SDL_QUIT: goto exit;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q: goto exit;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                if (x >= rect.x && x <= rect.x + 100
                        && y >= rect.y && y <= rect.y + 100) {
                    ++score;
                    hit = 1;
                    SDL_Log("hit");
                }
            }
            break;
        }

        SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
        SDL_RenderClear(renderer);

        char text[100];
        snprintf(text, 100, "Score: %d", score);
        SDL_Surface *message_surface = TTF_RenderText_Solid(font, text, white);
        SDL_Texture *message_texture = SDL_CreateTextureFromSurface(renderer, message_surface);
        SDL_FreeSurface(message_surface);
        SDL_RenderCopy(renderer, message_texture, 0, &message_rect);

        clock_gettime(CLOCK_MONOTONIC_RAW, &now);
        if (hit || now.tv_sec - last_rect.tv_sec > 0.1) {
            hit = 0;
            last_rect = now;
            rect.x = rand() % (800 - 100);
            rect.y = rand() % (600 - 100);
            rect.w = 100;
            rect.h = 100;
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);

        SDL_RenderPresent(renderer);
    }

exit:
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
