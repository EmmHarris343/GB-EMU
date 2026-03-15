#ifndef BASIC_VIEWER_H
#define BASIC_VIEWER_H

#include <SDL2/SDL.h>
#include "../core/video/video_debug.h"

typedef struct BasicViewer {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;

    DebugVideoSource source;
    DebugViewKind current_view;

    int window_scale;
} BasicViewer;

int basic_viewer_init(BasicViewer *viewer, DebugVideoSource source, DebugViewKind view_kind, int window_scale);
int basic_viewer_present(BasicViewer *viewer);
void basic_viewer_shutdown(BasicViewer *viewer);

#endif