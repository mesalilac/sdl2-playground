#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define HEXCOLOR(code)                                                                                                 \
    ((code) >> (3 * 8)) & 0xFF, ((code) >> (2 * 8)) & 0xFF, ((code) >> (1 * 8)) & 0xFF, ((code) >> (0 * 8)) & 0xFF

#define BACKGROUND_COLOR 0x181818FF
#define WIDTH 800
#define HEIGHT 450

#define PRIMARY_COLOR 0x337AB7FF
#define PROGRESSBAR_BODY_COLOR 0x202020FF
#define PROGRESSBAR_BODY_FILL_COLOR 0x337AB7FF

#define BUTTON_FG_COLOR 255, 255, 255
#define BUTTON_BG_COLOR PRIMARY_COLOR
#define BUTTON_BG_HOVER_COLOR 0x4b91cdFF
#define BUTTON_BG_CLICKED_COLOR 0x214f77FF

#define TOGGLE_BUTTON_WIDTH 70

/*
    TODO: Progress bar
    TODO: Buttons
    TODO: Toggle button

    TODO: Toggle button (Draggable)
    TODO: Horizontal slider
    TODO: Vertical slider
    TODO: counter
            <n> [-] [+]
    TODO: check button
    TODO: radio button
*/

typedef struct
{
    bool clicked;
    bool hover;
    bool on;
    bool actionPerformed;
    bool dragging;
} State;

void progress_bar(SDL_Renderer *ren, int x, int y, int min, int max, int value)
{
    if (value > max)
        value = max;
    else if (value < min)
        value = min;

    SDL_SetRenderDrawColor(ren, HEXCOLOR(PROGRESSBAR_BODY_COLOR));
    SDL_Rect body = {.x = x, .y = y, .w = max, .h = 20};
    SDL_RenderFillRect(ren, &body);

    SDL_SetRenderDrawColor(ren, HEXCOLOR(PROGRESSBAR_BODY_FILL_COLOR));
    SDL_Rect body_fill = {.x = x, .y = y, .w = value, .h = 20};
    SDL_RenderFillRect(ren, &body_fill);
}

void button(SDL_Renderer *ren, TTF_Font *font, State *btn_state, int x, int y, char *label)
{
    SDL_Color color = {BUTTON_FG_COLOR};
    SDL_Surface *surface = TTF_RenderText_Solid(font, label, color);
    SDL_Texture *message = SDL_CreateTextureFromSurface(ren, surface);

    int rect_color = BUTTON_BG_COLOR;

    if (btn_state->clicked)
    {
        rect_color = BUTTON_BG_CLICKED_COLOR;
    }
    else if (btn_state->hover)
    {
        rect_color = BUTTON_BG_HOVER_COLOR;
    }

    SDL_SetRenderDrawColor(ren, HEXCOLOR(rect_color));
    SDL_Rect rect = {
        .x = x,
        .y = y,
        .w = surface->w,
        .h = surface->h,
    };

    SDL_RenderFillRect(ren, &rect);
    SDL_RenderCopy(ren, message, NULL, &rect);

    int mouse_x, mouse_y;
    Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
    SDL_Point cursor = {mouse_x, mouse_y};

    if (!btn_state->clicked)
    {
        if (SDL_PointInRect(&cursor, &rect) && (buttons & SDL_BUTTON_LMASK) != 0)
        {
            btn_state->clicked = true;
            btn_state->actionPerformed = true;
        }

        if (SDL_PointInRect(&cursor, &rect) && (buttons & SDL_BUTTON_LMASK) == 0)
            btn_state->hover = true;
        else
        {
            btn_state->hover = false;
        }
    }
    else
    {
        if ((buttons & SDL_BUTTON_LMASK) == 0)
        {
            btn_state->clicked = false;
            btn_state->actionPerformed = false;
        }
    }

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(message);
}

void toggle_button(SDL_Renderer *ren, TTF_Font *font, State *state, int x, int y)
{
    SDL_SetRenderDrawColor(ren, HEXCOLOR(0x202020FF));
    SDL_Rect body = {.x = x, .y = y, .w = TOGGLE_BUTTON_WIDTH, .h = 25};
    SDL_RenderFillRect(ren, &body);
    SDL_Color color = {BUTTON_FG_COLOR};

    {
        SDL_SetRenderDrawColor(ren, HEXCOLOR(0x202020FF));
        SDL_Surface *surface = TTF_RenderText_Solid(font, "ON", color);
        SDL_Texture *message = SDL_CreateTextureFromSurface(ren, surface);
        SDL_Rect text_box = {.x = x, .y = y, .w = surface->w, .h = surface->h};

        SDL_RenderFillRect(ren, &text_box);
        SDL_RenderCopy(ren, message, NULL, &text_box);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(message);
    }
    {
        SDL_SetRenderDrawColor(ren, HEXCOLOR(0x202020FF));
        SDL_Surface *surface = TTF_RenderText_Solid(font, "OFF", color);
        SDL_Texture *message = SDL_CreateTextureFromSurface(ren, surface);
        SDL_Rect text_box = {.x = x + TOGGLE_BUTTON_WIDTH - surface->w, .y = y, .w = surface->w, .h = surface->h};

        SDL_RenderFillRect(ren, &text_box);
        SDL_RenderCopy(ren, message, NULL, &text_box);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(message);
    }

    SDL_Rect rect;

    SDL_SetRenderDrawColor(ren, HEXCOLOR(PRIMARY_COLOR));
    if (state->on == true)
    {
        rect.x = x + TOGGLE_BUTTON_WIDTH / 2;
        rect.y = y;
        rect.w = TOGGLE_BUTTON_WIDTH / 2;
        rect.h = 25;
    }
    else
    {
        rect.x = x;
        rect.y = y;
        rect.w = TOGGLE_BUTTON_WIDTH / 2;
        rect.h = 25;
    }

    SDL_RenderFillRect(ren, &rect);

    int mouse_x, mouse_y;
    Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
    SDL_Point cursor = {mouse_x, mouse_y};

    if (!state->clicked)
    {

        if (SDL_PointInRect(&cursor, &body) && (buttons & SDL_BUTTON_LMASK) != 0)
        {
            state->on = !state->on;
            state->clicked = true;
        }
    }
    else
    {
        if ((buttons & SDL_BUTTON_LMASK) == 0)
        {
            state->clicked = false;
        }
    }
}

#define HORIZONTAL_SLIDER_LEN 200.0f
#define HORIZONTAL_SLIDER_THICNESS 5.0f
#define HORIZONTAL_SLIDER_COLOR 0x202020FF
#define HORIZONTAL_SLIDER_GRIP_SIZE 10.0f
#define HORIZONTAL_SLIDER_GRIP_COLOR 0x76946AFF
#define HORIZONTAL_SLIDER_GRIP_DRAGGING_COLOR 0x5e7655ff
void horizontal_slider(SDL_Renderer *ren, int x, int y, State *state, int min, int max, float *value)
{
    SDL_Rect body = {.x = x,
                     .y = y - HORIZONTAL_SLIDER_THICNESS * 0.5f,
                     .w = HORIZONTAL_SLIDER_LEN,
                     .h = HORIZONTAL_SLIDER_THICNESS};

    SDL_SetRenderDrawColor(ren, HEXCOLOR(HORIZONTAL_SLIDER_COLOR));
    SDL_RenderFillRect(ren, &body);

    // slider grip
    float grip_value = *value / (max - min) * HORIZONTAL_SLIDER_LEN;
    SDL_SetRenderDrawColor(
        ren, HEXCOLOR(state->dragging == true ? HORIZONTAL_SLIDER_GRIP_DRAGGING_COLOR : HORIZONTAL_SLIDER_GRIP_COLOR));

    // Fill slider till grip_value
    {
        SDL_SetRenderDrawColor(ren, HEXCOLOR(HORIZONTAL_SLIDER_GRIP_COLOR));
        SDL_Rect rect = {
            .x = x, .y = y - HORIZONTAL_SLIDER_THICNESS * 0.5f, .w = grip_value, .h = HORIZONTAL_SLIDER_THICNESS};
        SDL_RenderFillRect(ren, &rect);
    }

    SDL_Rect slider_grip_rect = {
        .x = x - HORIZONTAL_SLIDER_GRIP_SIZE + grip_value,
        .y = y - HORIZONTAL_SLIDER_GRIP_SIZE,
        .w = HORIZONTAL_SLIDER_GRIP_SIZE * 2.0f,
        .h = HORIZONTAL_SLIDER_GRIP_SIZE * 2.0f,
    };
    SDL_RenderFillRect(ren, &slider_grip_rect);

    int mouse_x, mouse_y;
    Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

    if (!state->dragging)
    {
        SDL_Point cursor = {mouse_x, mouse_y};
        if (SDL_PointInRect(&cursor, &slider_grip_rect) && (buttons & SDL_BUTTON_LMASK) != 0)
        {
            state->dragging = true;
        }
    }
    else
    {
        if ((buttons & SDL_BUTTON_LMASK) == 0)
        {
            state->dragging = false;
        }
        else
        {
            float xf = mouse_x - HORIZONTAL_SLIDER_GRIP_SIZE;
            float grip_min = x - HORIZONTAL_SLIDER_GRIP_SIZE;
            float grip_max = x - HORIZONTAL_SLIDER_GRIP_SIZE + HORIZONTAL_SLIDER_LEN;

            if (xf < grip_min)
                xf = grip_min;
            if (xf > grip_max)
                xf = grip_max;

            *value = (xf - grip_min) / HORIZONTAL_SLIDER_LEN * (max - min) + min;
        }
    }
}

void vertical_slider()
{
}

/*
0[-][+]
*/
void counter(SDL_Renderer *ren, TTF_Font *font, int x, int y, int min, int max, int *count)
{
    // Subtract button
    State subtract_btn_state = {false};
    button(ren, font, &subtract_btn_state, x, y, "-");

    if (subtract_btn_state.clicked && subtract_btn_state.actionPerformed)
    {
        if (*count > min)
            *count -= 1;
        subtract_btn_state.actionPerformed = false;
    }

    // addition button
    State addition_btn_state = {false};
    button(ren, font, &addition_btn_state, x + 15, y, "+");

    if (addition_btn_state.clicked && addition_btn_state.actionPerformed)
    {
        if (*count < max)
            *count += 1;
        addition_btn_state.actionPerformed = false;
    }

    SDL_Color color = {255, 255, 255};

    char *str = malloc(100 * sizeof(char));
    sprintf(str, "%d", *count);
    SDL_Surface *surface = TTF_RenderText_Solid(font, str, color);
    SDL_Texture *message = SDL_CreateTextureFromSurface(ren, surface);

    SDL_Rect text_box = {
        .x = x + 30,
        .y = y,
        .w = surface->w,
        .h = surface->h,

    };
    SDL_SetRenderDrawColor(ren, HEXCOLOR(0x202020FF));
    SDL_RenderFillRect(ren, &text_box);
    SDL_RenderCopy(ren, message, NULL, &text_box);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(message);
    free(str);
}

void check_button()
{
}

void radio_button()
{
}

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "ERROR: Could not initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow("SDL-playground", 0, 0, WIDTH, HEIGHT, 0);
    if (win == NULL)
    {
        fprintf(stderr, "ERROR: Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (ren == NULL)
    {
        fprintf(stderr, "ERROR: Could not create renderer: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() < 0)
    {
        fprintf(stderr, "ERROR: Could not create TTF: %s", SDL_GetError());
        return 1;
    }

    TTF_Font *font = TTF_OpenFont("FiraCode-Regular.ttf", 18);

    bool quit = false;
    State btn_state = {false};
    State toggle_button_state = {false};
    int count = 0;
    State h_slider_state = {false};
    float h_slider = 1.0f;

    while (!quit)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                quit = true;
                break;
            }
        }

        SDL_SetRenderDrawColor(ren, HEXCOLOR(BACKGROUND_COLOR));
        SDL_RenderClear(ren);

        progress_bar(ren, 10, 10, 0, 100, count);
        button(ren, font, &btn_state, 10, 40, "Text");
        toggle_button(ren, font, &toggle_button_state, 10, 80);
        counter(ren, font, 10, 120, 0, 100, &count);
        horizontal_slider(ren, 10, 180, &h_slider_state, 1, 100, &h_slider);

        if (btn_state.clicked && btn_state.actionPerformed)
        {
            printf("clicked!\n");
            btn_state.actionPerformed = false;
        }

        SDL_RenderPresent(ren);
    }

    SDL_DestroyWindow(win);
    SDL_DestroyRenderer(ren);

    SDL_Quit();
    return 0;
}
