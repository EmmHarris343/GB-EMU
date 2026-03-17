#include <SDL2/SDL_error.h>
#include <SDL2/SDL_render.h>
#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..
#include "basic_viewer.h"

int basic_viewer_init(BasicViewer *viewer, DebugVideoSource source, DebugViewKind view_kind, int window_scale, SDL_PixelFormat gb_pixel_format) {
    DebugSurface surface;
    int window_width;
    int window_height;

    if (viewer == NULL) {
        printf("Viewer is null aborting..\n");
        return -1;
    }
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
    printf("What is the widnow height look like? %d\n", window_height);

    viewer->window = SDL_CreateWindow(
        "GB-EMU - Emulation - Test Graphics",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_SHOWN
    );

    if (!viewer->window) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        return -1;
    }

    viewer->renderer = SDL_CreateRenderer(viewer->window , -1, SDL_RENDERER_SOFTWARE);
    if (!viewer->renderer) {
        fprintf(stderr, "Renderer Null; Failure creating renderer. Error: %s\n", SDL_GetError());
        return -1;
    }

    viewer->pixel_format = &gb_pixel_format;
    if (viewer->pixel_format == NULL) {
        fprintf(stderr, "SDL_AllocFormat failed: %s\n", SDL_GetError());
        return -1;
    }

    viewer->texture = SDL_CreateTexture(
        viewer->renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        surface.width,
        surface.height
    );
    if (viewer->texture == NULL) {
        fprintf(stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());
        return -1;
    }

    // // Might help reduce alpha blend modes?
    // SDL_SetTextureBlendMode(viewer->texture, SDL_BLENDMODE_NONE);

    printf("Finished initializing Basic Viewer\n");
    return 0;
}

int basic_viewer_present(BasicViewer *viewer) {
    DebugSurface surface;
    int pitch_bytes;

    if (viewer == NULL) {
        return -1;
    }

    if (viewer->source.ops.get_surface(viewer->source.ctx, viewer->current_view, &surface) != 0) {
        printf("Failed to get surface from adapter/ video_debug?\n");
        return -1;
    }

    pitch_bytes = surface.pitch_pixels * sizeof(uint32_t);
    //pitch_bytes = surface.pitch_pixels * (int)sizeof(uint32_t);   // Old way, might not be working.

    if (SDL_UpdateTexture(
        viewer->texture,
        NULL,
        surface.pixels,
        pitch_bytes  // old way. might be wrong.
        //pitch_bytes * sizeof(uint32_t)    // Causes seg fault..
        ) != 0 ) {
        printf("Failed to update texture?\n");
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
    // if (viewer->pixel_format != NULL) {
    //     SDL_FreeFormat(viewer->pixel_format);
    //     viewer->pixel_format = NULL;
    // }

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