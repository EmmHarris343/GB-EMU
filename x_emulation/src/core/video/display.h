#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL_pixels.h>
#include <stdint.h>

typedef struct ppu_s PPU;

struct SDL_PixelFormat;

#define GB_LCD_WIDTH   160
#define GB_LCD_HEIGHT  144
#define GB_BG_WIDTH    256
#define GB_BG_HEIGHT   256

#define VRAM_TILE_8000_OFFSET  0x0000
#define VRAM_TILE_9000_OFFSET  0x1000
#define VRAM_BG_OFFSET_9800    0x1800
#define VRAM_BG_OFFSET_9C00    0x1C00

typedef struct DisplaySurface {
    uint32_t *pixels;
    int width;
    int height;
    int pitch_pixels;
} DisplaySurface;

typedef enum ViewKind {
    VIEW_PORT,
    DEBUG_VIEW_LCD,
    DEBUG_VIEW_BG_MAP
} ViewKind;

typedef struct Surface {
    uint32_t *pixels;
    int width;
    int height;
    int pitch_pixels;
} Surface;

typedef struct VideoOps {
    int (*get_surface)(void *ctx, ViewKind view_kind, Surface *out_surface);
} VideoOps;

typedef struct VideoSource VideoSource;

struct VideoSource {
    void *ctx;
    VideoOps ops;
};

void gen_pixel_line(PPU *ppu, const uint8_t *vram);

#endif