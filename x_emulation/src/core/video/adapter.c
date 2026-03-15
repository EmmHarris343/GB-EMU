#include "adapter.h"
#include "../gb.h"

static int gb_debug_get_surface(void *ctx, DebugViewKind view_kind, DebugSurface *out_surface) {
    GB *gb = ctx;

    if (out_surface == NULL) {
        return -1;
    }

    switch (view_kind) {
        case DEBUG_VIEW_LCD:
            out_surface->pixels = gb->ppu.debug_lcd_rgba;
            out_surface->width = GB_LCD_WIDTH;
            out_surface->height = GB_LCD_HEIGHT;
            out_surface->pitch_pixels = GB_LCD_WIDTH;
            return 0;

        case DEBUG_VIEW_BG_MAP:
            out_surface->pixels = gb->ppu.debug_bg_rgba;
            out_surface->width = GB_BG_WIDTH;
            out_surface->height = GB_BG_HEIGHT;
            out_surface->pitch_pixels = GB_BG_WIDTH;
            return 0;

        default:
            return -1;
    }
}

void video_init_source(GB *gb, DebugVideoSource *out_source) {
    out_source->ctx = gb;
    out_source->ops.get_surface = gb_debug_get_surface;
}