#ifndef VIDEO_DEBUG_H
#define VIDEO_DEBUG_H

#include <SDL2/SDL_pixels.h>
#include <stdint.h>

typedef struct ppu_s PPU;

struct SDL_PixelFormat;

#define GB_LCD_WIDTH   160
#define GB_LCD_HEIGHT  144
#define GB_BG_WIDTH    256
#define GB_BG_HEIGHT   256

typedef enum DebugViewKind {
    DEBUG_VIEW_LCD,
    DEBUG_VIEW_BG_MAP
} DebugViewKind;

typedef struct DebugSurface {
    uint32_t *pixels;
    int width;
    int height;
    int pitch_pixels;
} DebugSurface;

typedef struct DebugVideoSource DebugVideoSource;

typedef struct DebugVideoOps {
    int (*get_surface)(void *ctx, DebugViewKind view_kind, DebugSurface *out_surface);
} DebugVideoOps;

struct DebugVideoSource {
    void *ctx;
    DebugVideoOps ops;
};

void build_test_pattern(PPU *ppu, SDL_PixelFormat *gb_pixel_format);

void build_debug_test_bg(PPU *ppu, const uint8_t *vram, SDL_PixelFormat *gb_pixel_format);
void build_bg_window(PPU *ppu, const uint8_t *vram, SDL_PixelFormat *gb_pixel_format);

#endif