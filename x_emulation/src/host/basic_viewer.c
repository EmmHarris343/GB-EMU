#include <SDL2/SDL_error.h>
#include <SDL2/SDL_render.h>
#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..
#include "basic_viewer.h"

int basic_viewer_init(BasicViewer *viewer, DebugVideoSource source, DebugViewKind view_kind, int window_scale) {
    DebugSurface surface;
    int window_width;
    int window_height;

    if (viewer == NULL) {
        printf("Viewer is null aborting..\n");
        return -1;
    }

    SDL_Renderer *rendererobj;

    viewer->window = NULL;
    viewer->renderer = NULL;
    viewer->texture = NULL;
    viewer->source = source;
    viewer->current_view = view_kind;
    viewer->window_scale = window_scale;

    if (viewer->source.ops.get_surface(viewer->source.ctx, viewer->current_view, &surface) != 0) {
        printf("Couldn't get viewer source/surface. Aborting..\n");
        return -1;
    }


    window_width = surface.width * window_scale;
    window_height = surface.height * window_scale;
    printf("What is the widnow heightl look like? %d\n", window_height);

    viewer->window = SDL_CreateWindow(
        "GB-EMU - Emulation - Test Graphics",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_SHOWN
    );

    // SDL_Window *window = SDL_CreateWindow(
    //     "Gameboy Emulation - Test Graphics",
    //     SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    //     SCREEN_WIDTH * 4, SCREEN_HEIGHT *4,     // Scale up window :: Width: 580 X Height: 510
    //     SDL_WINDOW_SHOWN);


    if (viewer->window == NULL) {
        printf("Window is null. Aborting..\n");
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        return -1;
    }

    fprintf(stderr, "Renderer Null; Failure creating renderer. Error: %s\n", SDL_GetError());

    rendererobj = SDL_CreateRenderer(viewer->window, -1, SDL_RENDERER_ACCELERATED);
    if (rendererobj == NULL) {
        fprintf(stderr, "Renderer Null; Failure creating renderer. Error: %s\n", SDL_GetError());
        return -1;
    }

    // viewer->texture = SDL_CreateTexture(
    //     viewer->renderer,
    //     SDL_PIXELFORMAT_RGBA8888,
    //     SDL_TEXTUREACCESS_STREAMING,
    //     surface.width,
    //     surface.height
    // );
    // if (viewer->texture == NULL) {
    //     printf("Texture is null. Aborting..\n");
    //     fprintf(stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());
    //     return -1;
    // }

    return 0;
}

int basic_viewer_present(BasicViewer *viewer) {
    DebugSurface surface;
    int pitch_bytes;

    if (viewer == NULL) {
        return -1;
    }

    if (viewer->source.ops.get_surface(viewer->source.ctx, viewer->current_view, &surface) != 0) {
        return -1;
    }

    pitch_bytes = surface.pitch_pixels * (int)sizeof(uint32_t);

    if (SDL_UpdateTexture(viewer->texture, NULL, surface.pixels, pitch_bytes) != 0) {
        return -1;
    }

    SDL_RenderClear(viewer->renderer);
    SDL_RenderCopy(viewer->renderer, viewer->texture, NULL, NULL);
    SDL_RenderPresent(viewer->renderer);

    return 0;
}

void basic_viewer_shutdown(BasicViewer *viewer) {
    if (viewer == NULL) {
        return;
    }

    if (viewer->texture != NULL) {
        SDL_DestroyTexture(viewer->texture);
        viewer->texture = NULL;
    }

    if (viewer->renderer != NULL) {
        SDL_DestroyRenderer(viewer->renderer);
        viewer->renderer = NULL;
    }

    if (viewer->window != NULL) {
        SDL_DestroyWindow(viewer->window);
        viewer->window = NULL;
    }
}