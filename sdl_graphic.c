// build by:
// gcc sdl_graphic.c -o sdl_graphic -lSDL2 -lm -Wall -Werror


#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..
#include <stdio.h>
#include <stdlib.h>

// graphic specific:
#include <SDL2/SDL.h>
#include <stdio.h>

#define SCREEN_WIDTH        160
#define SCREEN_HEIGHT       144

// Logo data:
#define b16_LOGO            24
#define LOGO_B_LNGTH        48
#define LOGO_WIDTH          48
#define LOGO_HEIGHT         8
#define HEX_LOGO_START          0x0104
#define HEX_LOGO_END            0x0134


// LOGO HEX DUMP:
// |TOP| CE ED 66 66 CC 0D 00 0B 03 73 00 83 00 0C 00 0D 00 08 11 1F 88 89 00 0E
// |Btm| DC CC 6E E6 DD DD D9 99 BB BB 67 63 6E 0E EC CC DD DC 99 9F BB B9 33 3E


uint8_t raw_logo_data[(HEX_LOGO_END -1) - HEX_LOGO_START];      // -1, cause it only needs to be 0-47 not 0-48

int load_logo_from_rom(const char *filename) {
    FILE *rom = fopen(filename, "rb");
    if (!rom) {
        printf("error: cannot open Rom file!\n");
        return -1;
    }

    fseek(rom, HEX_LOGO_START, SEEK_SET);
    fread(raw_logo_data, 1, HEX_LOGO_END - HEX_LOGO_START, rom);

    fclose(rom);

    return 0;
}

void decode_colour_tile(uint8_t *left_input_byte_lsb, uint8_t *right_byte_msb, uint16_t *decoded_color) {
    // Extract the 2 bytes for the logo colour data - then interweave
    
    int wv_loop_count = 0;  // I'm trying to avoid divides, so add another int to count to..
    for (int byte_count = 0; byte_count < LOGO_WIDTH; byte_count++){
        if (byte_count > 0 && byte_count % 2 > 0) {
            
            uint8_t lsb = raw_logo_data[byte_count -1];
            uint8_t msb = raw_logo_data[byte_count];
            uint16_t working_logo_data = 0;
            
            // Values are read in the reverse value, so loop from largest to smallest
            for (int bit_val = 7; bit_val >= 0; bit_val--){
                uint8_t msb_bit =  (msb >> bit_val) & 1;  // The & 1 ensures we only take 1 bit
                uint8_t lsb_bit = (lsb >> bit_val) & 1;

                // MSB Bit first, LSB Second. ia, jb, kc, ld, me, nf, og, ph.
                working_logo_data <<= 1;
                working_logo_data |= msb_bit;
                            
                working_logo_data <<= 1;
                working_logo_data |= lsb_bit;

                // printf("lsb_bit: %02d (Pass): %02d | \n", lsb_bit, bit_val);
                // printf("msb_bit: %02d (Pass): %02d | ", msb_bit, bit_val);
            }
            printf("\n |16bit| Pixel Data (IN HEX):: %02X :: ", working_logo_data);
            
            
            decoded_color[wv_loop_count] = working_logo_data;  // This updates the pointer and returns it..
            wv_loop_count ++;   // Advance the count. Makes sure this stays 0 - 24
            
            //  
            // for (int j = 15; j >= 0; j--) {
            //     printf("%d", (i_pixel_val >> j) & 1);
            // }
        }

        if (byte_count == 47) {
            printf("\n -- Bye -- \n");
        }
    }
}


void render_tile(uint16_t *bit16_tile) {
    uint16_t bit16_logo[24];

    //uint16_t store_bit[15];

    for (int top_logo = 0; top_logo < 11; top_logo++) {
        
        uint16_t converted_colour = bit16_logo[top_logo];
        printf("Working on colour byte: %02X\n", bit16_logo[top_logo]);
        for (int b_bit = 15; b_bit >= 0; b_bit--){
            if ((b_bit % 2) < 1) {  // Multiple of 2 only
                uint16_t colour_2bit =  (converted_colour >> b_bit ) & 0x03; // & 2, means max of binary b xx10, not b xx 11. 3 allows it to move up to 11
                //store_bit[b_bit] = colour_2bit;
                printf("Colour 2bit is... what? :: %d ::\n", colour_2bit);
                switch(colour_2bit){
                    case 0x00: // white:
                        printf("White\n");
                        //pixels[b_bit] = 0xFFFFFFFF;      // this is not the right value for the pixel, but fun to visually see it maybe regardless?
                        break;
                    case 0x01:
                        printf("Light Grey\n");
                        //pixels[b_bit] = 0xFFAAAAAA;
                        break;
                    case 0x02:
                        printf("Dark Grey\n");
                        //pixels[b_bit] = 0xFF555555;
                        break;
                    case 0x03: // Black:
                        printf("Black\n");
                        //pixels[b_bit] = 0xFF000000;
                        break;
                    default:
                        printf("Default.. to white\n");
                        //pixels[b_bit] = 0xFFFFFFFF;
                        break;
                }
                printf("Color?? %02X\n", colour_2bit);
            }
        }
    }


}



void get_16BIT(uint16_t *INTWV_16b_LOGO) {
    // Extract the 2 bytes for the logo colour data - then interweave
    
    int wv_loop_count = 0;  // I'm trying to avoid divides, so add another int to count to..
    for (int byte_count = 0; byte_count < LOGO_WIDTH; byte_count++){       

        // Ensure we process 2 Byte data, ONLY when we land on the second Byte.
        if (byte_count > 0 && byte_count % 2 > 0) {
            
            uint8_t lsb = raw_logo_data[byte_count -1];
            uint8_t msb = raw_logo_data[byte_count];
            uint16_t working_logo_data = 0;
            
            // Values are read in the reverse value, so loop from largest to smallest
            for (int bit_val = 7; bit_val >= 0; bit_val--){
                uint8_t msb_bit =  (msb >> bit_val) & 1;  // The & 1 ensures we only take 1 bit
                uint8_t lsb_bit = (lsb >> bit_val) & 1;

                // MSB Bit first, LSB Second. 
                // ml ml
                // ml ml

                // If -> Pixel is lit if the corresponding bit is set.
                working_logo_data <<= 1;
                working_logo_data |= msb_bit;
                            
                working_logo_data <<= 1;
                working_logo_data |= lsb_bit;

                // printf("lsb_bit: %02d (Pass): %02d | \n", lsb_bit, bit_val);
                // printf("msb_bit: %02d (Pass): %02d | ", msb_bit, bit_val);                
            }
            printf("\n |16bit| Pixel Data (IN HEX):: %02X :: ", working_logo_data);
            
            
            INTWV_16b_LOGO[wv_loop_count] = working_logo_data;  // This updates the pointer and returns it..
            wv_loop_count ++;   // Advance the count. Makes sure this stays 0 - 24
            
            //  
            // for (int j = 15; j >= 0; j--) {
            //     printf("%d", (i_pixel_val >> j) & 1);
            // }
        }

        if (byte_count == 47) {
            printf("\n -- Bye -- \n");
        }
    }


}


void get_LOGO() {
    // uint8_t *logo_top, uint8_t *logo_btm
    // Pass Logo Data from ROM into this function
    // Each 8 Bit Address (IE: CD, ED, ...) Will be split in half. <--|-->
    // Left-> Top, Right-> Bottom.

    // Have Column Loop, First. With Row Inside.
    // This will make it so it processes 0-7 Rows, (Maybe only 0-3) Under each Column at a time.

    uint8_t top_logo[24];
    uint8_t btm_logo[24];

    // Extract data, top / bottom

    printf("Last Byte Value #2: %02X\n", raw_logo_data[46]);
    
    int top_count = 0;
    int btm_count = 0;
    for (int byte_count = 0; byte_count < LOGO_WIDTH; byte_count++){

        if (byte_count < 24) {
            // Top
            top_logo[top_count] = raw_logo_data[byte_count];
            top_count ++;
        }
        if (byte_count > 24) {
            // Bottom
            btm_logo[btm_count] = raw_logo_data[byte_count];
            btm_count ++;
        }
    }
    if (btm_logo[0]) { printf("Btm_Logo has data.. (This is to get rid of annoying error...)\n");}



    printf("\n");

    //uint32_t pixels[LOGO_WIDTH * LOGO_HEIGHT * 8];      // 8x Larger due to 1-bit to 8-bit conversion.

    int data_actual = 0;
    int col_offset = 0;

    uint16_t store_row = 0;
    
    

    for (int l_col = 0; l_col < 12; l_col++){
        store_row = 0;
        uint8_t temp_bits[4] = {0};

        col_offset = 0;
        data_actual = (l_col * 2);

        
        //printf("|| COL:%d || BIT0: %02X BIT1: %02X|", l_col, top_logo[data_actual], top_logo[data_actual+1]);
        
        for (int l_row = 0; l_row < 4; l_row++) {
            if (l_row == 2) col_offset = 1; // Nudge forward column offset


            if (l_row == 0 || l_row == 2) {
                temp_bits[l_row] = top_logo[data_actual + col_offset] >> 4;
                //printf("|BIT| %02X", top_logo[data_actual + col_offset] >> 4);
            }
            else {
                temp_bits[l_row] = top_logo[data_actual + col_offset] & 0xf;
                //printf("|BIT| %02X", top_logo[data_actual + col_offset] & 0xf);
            }
        }
        store_row |= (temp_bits[0] << 12);  // Row 0 (top-most)
        store_row |= (temp_bits[1] << 8);   // Row 1
        store_row |= (temp_bits[2] << 4);   // Row 2
        store_row |= temp_bits[3];          // Row 3

        //pixels[l_col] = store_row;        // Unused RIGHT NOW. So comment out.
        printf("COL:%2d | %04X\n", l_col, store_row);
    }
    //printf("\n");
}



void render_logo(SDL_Renderer *renderer, SDL_Texture *texture, uint16_t *bit16_logo) {
    uint32_t pixels[LOGO_WIDTH * LOGO_HEIGHT * 8];
    uint8_t top_logo[24];
    uint8_t btm_logo[24];

    // Extract data, top / bottom    
    int top_count = 0;
    int btm_count = 0;
    for (int byte_count = 0; byte_count < LOGO_WIDTH; byte_count++){

        if (byte_count < 24) {
            // Top
            top_logo[top_count] = raw_logo_data[byte_count];
            top_count ++;
        }
        if (byte_count >= 24) {
            // Bottom
            btm_logo[btm_count] = raw_logo_data[byte_count];
            btm_count ++;
        }
    }

    uint32_t pixels_raw[LOGO_WIDTH * LOGO_HEIGHT];      // Not upscaling.

    int data_actual = 0;
    int col_offset_t = 0;
    int col_offset_b = 0;
    uint32_t store_row = 0;
  
    for (int l_col = 0; l_col < 12; l_col++){
        store_row = 0;
        uint8_t temp_bits[8] = {0};
        col_offset_t = 0;
        col_offset_b = 0;
        data_actual = (l_col * 2);

        for (int l_row = 0; l_row < 8; l_row++) {
            if (l_row < 4) {
                if (l_row == 2) col_offset_t = 1; // Nudge forward column offset                
                if (l_row == 0 || l_row == 2) {
                    temp_bits[l_row] = (top_logo[data_actual + col_offset_t]) >> 4;
                }
                else {
                    temp_bits[l_row] = (top_logo[data_actual + col_offset_t]) & 0xF;
                }
            }
            if (l_row >= 4) {
                if (l_row == 6) col_offset_b = 1; // Nudge forward column offset
                if (l_row == 4 || l_row == 6) {
                    temp_bits[l_row] = (btm_logo[data_actual + col_offset_b] >> 4) & 0xF;
                }
                else {
                    temp_bits[l_row] = (btm_logo[data_actual + col_offset_b]) & 0xF;
                }
            }
        }
        store_row |= (temp_bits[0] & 0XF) << 28;   // Row 0 (top-most)
        store_row |= (temp_bits[1] & 0XF) << 24;   // Row 1
        store_row |= (temp_bits[2] & 0XF) << 20;   // Row 2
        store_row |= (temp_bits[3] & 0XF) << 16;   // Row 3
        store_row |= (temp_bits[4] & 0XF) << 12;   // Row 4 (top of btm-row)
        store_row |= (temp_bits[5] & 0XF) << 8;    // Row 5
        store_row |= (temp_bits[6] & 0XF) << 4;    // Row 6
        store_row |= (temp_bits[7] & 0XF);         // Row 7

        pixels_raw[l_col] = store_row;
    }



    int bitgroup = 0;
    int pixel_index = 0;
    uint32_t pixels_storage = 0;
    for (int row = 0; row < LOGO_HEIGHT; row++) {
        bitgroup = 0;
        for (int col = 0; col < LOGO_WIDTH; col++) {    // 48 is accurate, Because 4bit per column * 12 = 48
            if (col > 3 && (col) % 4 == 0) bitgroup++; // it's hit a 4 group...
            int bitrow_loc[4] = {3, 2, 1, 0};
            int bitgroup_loc[8] = {28, 24, 20, 16, 12, 8, 4, 0};
            pixel_index = (LOGO_WIDTH * row) + col;


            pixels_storage = pixels_raw[bitgroup];
            uint32_t bitrow_val = (pixels_storage >> bitgroup_loc[row]) & 0xf;  // >> Left by 4, grabbing 4 bits
            uint8_t bit_val = (bitrow_val >> bitrow_loc[col % 4]) & 1;
            /*
            Using (mod) col % 4, This will always return 0,1,2,3 
            The reversed "bitrow_loc" array values. Tells it how many bits to shift Left.
            >> by 3, then 2, then 1, then 0. 
            Ensuring only 1 bit (the rightmost) is assigned to "bit_val"
            */

            // If bit_val is 1 or 0, Assign *White or *Black. To the value at the pixel index.
            pixels[pixel_index] = (bit_val > 0) ? 0xFFFFFFFF : 0xFF000000;
        }
    }




    // "Update a texture Rectangle with pixel data.  ---"
    // "The last value being the Pitch: Number of bytes in a row of pixel data, (Including padding between Lines)"
    SDL_UpdateTexture(texture, NULL, pixels, LOGO_WIDTH * sizeof(uint32_t)); // sizeof(uint32_t) = 4?
    
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}




int main() {
    // Startup, load rom, read logo data..
    char *rom_file = "rom/wrio_land_2.gb";
    printf("Using rom file: %s\n", rom_file);

    if (load_logo_from_rom(rom_file) < 0) {
        printf("Failure reading logo from rom file..");
        return 1;
    }
    printf("Get logo..\n");

    uint16_t INTWV_16b_LOGO[24];
    get_LOGO();
    printf("Got 16bit LOGO Data\n");

    // Initialize window, layout pixel format size etc.
    if (SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        printf("Failure, SDL could not initialize! SDL_ERROR: %s\n ", SDL_GetError());
        return -1;
    }


    SDL_Window *window = SDL_CreateWindow(
        "Gameboy Emulation - Test Graphics",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH * 4, SCREEN_HEIGHT *4,     // Scale up window :: Width: 580 X Height: 510
        SDL_WINDOW_SHOWN);
    
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    
    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, 
        -1,
        SDL_RENDERER_ACCELERATED
    );
    SDL_Texture *texture = SDL_CreateTexture(
        renderer, 
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        LOGO_WIDTH, LOGO_HEIGHT * 6         // This will make texture Width 0-47... BY 0-39 (8*8)
    );
    // NOTE, changing the Height value, Changes the shape of the Texture. 
    // Smaller Hight makes it more stretched like a rectangle. 
    // Making it Bigger, (Well errors out.. But.. Also makes it more square)

    // Doing LOGO_Height * 6. Looks pretty nice and square.


    render_logo(renderer, texture, INTWV_16b_LOGO);



    SDL_Delay(4000);        // Display for only 2 seconds (LIkely similar to wait(2000)..)

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}