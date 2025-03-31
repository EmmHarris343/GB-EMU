#include <stdio.h>
#include <stdlib.h>

/*
Notice - Using the google font: Economica
https://fonts.google.com/specimen/Economica

Can be downloaded there, thought would be fun to make the output font a little quirky (may remove later)

*/



// graphic specific:
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// Economica-Regular

// FILE *Econimica_font;


// int get_font() {
//     const char *filename = "./Economica-Regular.ttf";
//     FILE *Econimica_font = fopen(filename, "rb");
//     if (!Econimica_font) {
//         printf("error: cannot open Rom file!\n");
//         return -1;
//     }
// }


#define MAX_LOG_LINES 1000
#define MAX_LINE_LENGTH 256


// This is barebones for now, mostly to see if it can "work"

// The goal is so it can print out multiple lines quickly, instead of system calls to Printf() which will slow down emulation.

char logBuffer[MAX_LOG_LINES][MAX_LINE_LENGTH];
int logLineCount = 0;

SDL_Window *logWindow;
SDL_Renderer *logRenderer;

SDL_Color White = {255, 255, 255, 255};
TTF_Font *font;

void add_log_line(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(logBuffer[logLineCount % MAX_LOG_LINES], MAX_LINE_LENGTH, format, args);
    va_end(args);
    logLineCount++;
}


void init_window() {
    
    SDL_Window *logWindow = SDL_CreateWindow("Emulation Log", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 400, 0);
    SDL_Renderer *logRenderer = SDL_CreateRenderer(logWindow, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font *font = TTF_OpenFont("./Economica-Regular.ttf", 14);

}



void render_window() {
    uint8_t visibleLines = 1000;
    uint8_t lineHeight = 20;
    for (int i = 0; i < visibleLines; ++i) {
        int index = (logLineCount - visibleLines + i + MAX_LOG_LINES) % MAX_LOG_LINES;
        SDL_Surface *textSurface = TTF_RenderText_Solid(font, logBuffer[index], White);
        
        // SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(logRenderer, textSurface);

        
        SDL_Rect destRect = {5, i * lineHeight, textSurface->w, textSurface->h};
        SDL_RenderCopy(logRenderer, textTexture, NULL, &destRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
}


void log_output_startup() {
    init_window();
    render_window();
    add_log_line("Log Window Initiated");

}