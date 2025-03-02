// build by:
// gcc test_graphic.c -o sdl_graphic -lSDL2 -lm


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
#define LOGO_START          0x0104
#define LOGO_END            0x0133


// LOGO HEX DUMP:
// |TOP| CE ED 66 66 CC 0D 00 0B 03 73 00 83 00 0C 00 0D 00 08 11 1F 88 89 00 0E
// |Btm| DC CC 6E E6 DD DD D9 99 BB BB 67 63 6E 0E EC CC DD DC 99 9F BB B9 33 3E

// CE ED:
// CE: |MSB-SIDE| 1100 1110 |LSB-SIDE
// ED: |MSB-SIDE| 1110 1101 |LSB-SIDE

// OK. I do not need to weave them.. Just that each byte. Needs to be split in half.

/*
-- CE:
XX..
XXX.
-- ED:
xxx.
xx.x

*/

// if (!(byte_count & 1)) {    // Odd Vs Even (Method 2)



uint8_t raw_logo_data[LOGO_END - LOGO_START];

int load_logo_from_rom(const char *filename) {
    FILE *rom = fopen(filename, "rb");
    if (!rom) {
        printf("error: cannot open Rom file!\n");
        return -1;
    }

    fseek(rom, LOGO_START, SEEK_SET);
    fread(raw_logo_data, 1, LOGO_END - LOGO_START, rom);
    fclose(rom);

    return 0;
}

void decode_colour_tile(uint8_t *left_input_byte_lsb, uint8_t *right_byte_msb, uint16_t *decoded_color) {
    // Extract the 2 bytes for the logo colour data - then interweave
    
    uint16_t final_logo_data[24];   // Initialize to zero
    uint8_t lsb_byte[24];    // Less Significant (Left)
    uint8_t msb_byte[24];    // Most Significant (Right)
    

    int wv_loop_count = 0;  // I'm trying to avoid divides, so add another int to count to..
    for (int byte_count = 0; byte_count < LOGO_WIDTH; byte_count++){
        uint8_t i_byte = raw_logo_data[byte_count];
        // if (!(byte_count & 1)) {    // Find which is Even, Which is odd ### Method two ###

        // Ensure we process 2 Byte data, ONLY when we land on the second Byte.
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
    uint32_t pixels[LOGO_WIDTH * LOGO_HEIGHT * 8];      // 8x Larger due to 1-bit to 8-bit conversion.

    pixels[0] = 0xFFFFFFFF;     // Just make this pixel white for now...

    uint16_t bit16_logo[24];

    uint16_t single_colour_data;
    uint8_t stored_colour;
    uint8_t logo_2bit[12];

    uint16_t store_bit[15];

    for (int top_logo = 0; top_logo < 11; top_logo++) {
        
        uint16_t converted_colour = bit16_logo[top_logo];
        printf("Working on colour byte: %02X\n", bit16_logo[top_logo]);
        
        int pos_count = 0;
        for (int b_bit = 15; b_bit >= 0; b_bit--){
            if ((b_bit % 2) < 1) {  // Multiple of 2 only
                uint16_t colour_2bit =  (converted_colour >> b_bit ) & 0x03; // & 2, means max of binary b xx10, not b xx 11. 3 allows it to move up to 11
                store_bit[b_bit] = colour_2bit;
                printf("Colour 2bit is... what? :: %d ::\n", colour_2bit);
                switch(colour_2bit){
                    case 0x00: // white:
                        printf("White\n");
                        pixels[b_bit] = 0xFFFFFFFF;      // this is not the right value for the pixel, but fun to visually see it maybe regardless?
                        break;
                    case 0x01:
                        printf("Light Grey\n");
                        pixels[b_bit] = 0xFFAAAAAA;
                        break;
                    case 0x02:
                        printf("Dark Grey\n");
                        pixels[b_bit] = 0xFF555555;
                        break;
                    case 0x03: // Black:
                        printf("Black\n");
                        pixels[b_bit] = 0xFF000000;
                        break;
                    default:
                        printf("Default.. to white\n");
                        pixels[b_bit] = 0xFFFFFFFF;
                        break;
                }
                printf("Color?? %02X\n", colour_2bit);
            }
        }
    }


}



void get_16BIT(uint16_t *INTWV_16b_LOGO) {
    // Extract the 2 bytes for the logo colour data - then interweave
    
    uint16_t final_logo_data[24];   // Initialize to zero
    uint8_t lsb_byte[24];    // Less Significant (Left)
    uint8_t msb_byte[24];    // Most Significant (Right)
    

    int wv_loop_count = 0;  // I'm trying to avoid divides, so add another int to count to..
    for (int byte_count = 0; byte_count < LOGO_WIDTH; byte_count++){
        uint8_t i_byte = raw_logo_data[byte_count];

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
    
    int top_count = 0;
    int btm_count = 0;
    for (int byte_count = 0; byte_count < LOGO_WIDTH; byte_count++){
        uint8_t i_byte = raw_logo_data[byte_count];

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



    printf("\n");

    uint32_t pixels[LOGO_WIDTH * LOGO_HEIGHT * 8];      // 8x Larger due to 1-bit to 8-bit conversion.

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

        pixels[l_col] = store_row;
        printf("COL:%2d | %04X\n", l_col, store_row);
    }
    //printf("\n");
}


            // // THIS ONLY PRINTS:
            // if (l_row == 0 || l_row == 2) {
            //     printf("< |dc:%d|OF:%d| B0: %02X >", data_actual, col_offset, (top_logo[data_actual + col_offset] >> 4));
            //     // Shift bytes over so there is only 4 bits selected. (1111 XXXX) -> (XXXX 1111)
            // }
            // else {
            //     printf("< |dc:%d|OF:%d| B1: %02X >", data_actual, col_offset, (top_logo[data_actual + col_offset] & 0xf));
            //     // Select only the first 4 bits up to value 0xf (15) (XXXX 1111)
            // }
            // if (l_row == 3){
            //     // Only needed when printing to console..
            //     printf("\n");
            // }

        //int pixel_index = (row * 8 + bit) * LOGO_WIDTH + l_col;


        // Now save this to something kind of variable?
        // int pixel_index = (row * 8 + bit) * LOGO_WIDTH + col;       // Exmple: index.. (row (3) * 8 + bit (2) = 66) 
        // uint8_t data = (byte & (1 << (7 - 1)));
        // pixels[pixel_index] = (byte & (1 << (7 - 1))) ? 0xFFFFFFFF : 0xFF000000;    // if bit true, make white. else make black








void render_logo(SDL_Renderer *renderer, SDL_Texture *texture, uint16_t *bit16_logo) {
    uint32_t pixels[LOGO_WIDTH * LOGO_HEIGHT * 8];
    // 8x Larger due to 1-bit to 8-bit conversion ... but why?

    pixels[0] = 0xFFFFFFFF;     // Just make this pixel white for now...
    pixels[47] = 0xFFFFFFFF;    // This shows the LAST pixel is at Column 47
    pixels[48] = 0xFFFFFFFF;    // This is Row #2 Start point

    // So... lets say it's
    // 0-47 Columns BY
    // 0-40 Rows (not completely true but you know..)


    // Needs to be.. At MIN 8 ROWS in Height.
    // and 12 * 4(bits) in COLUMNS Width. 









    //

    // -- OK -- 
    // Single Pixel WIDTH is: 0-47 Long. 
    
    // Width of "Texture" is 48 * sizeof(uint32_t) 4 = 120 Pixel widths..

    // Window/ Screen Width X Height = 580 X 510

    // 120 Pixel Width. 
    // If I don't upscale it. Then it's 1/4 width of "screen".

    uint8_t top_logo[24];
    uint8_t btm_logo[24];

    // Extract data, top / bottom
    
    int top_count = 0;
    int btm_count = 0;
    for (int byte_count = 0; byte_count < LOGO_WIDTH; byte_count++){
        uint8_t i_byte = raw_logo_data[byte_count];

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



    printf("\n");

    uint32_t pixels[LOGO_WIDTH * LOGO_HEIGHT * 8];      // 8x Larger due to 1-bit to 8-bit conversion.
    uint32_t pixels_raw[LOGO_WIDTH * LOGO_HEIGHT];      // not multiplying... should be enough..

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

        pixels_raw[l_col] = store_row;
        printf("COL:%2d | %04X\n", l_col, store_row);
    }
    //printf("\n");


    // 


    


    int col_4bitgroup = 0;
    // There is actually 12 columns... 

    // pixels_raw[0-11] Has a 16Bit value, with 2 * 8 bit values squished together.
    // IE: CE & ED = CEED

    // I need to split, C, E, E, D .. Into the 4 bit gropus. 
    // Reading on this FIRST-ROW only pixels_raw[0] Left-Most bit (C), 
    // NEXT ROW, I need to Read Pixels_raw[0] Second-Left-Most Bit (E), 
    // 


    // So..... Put Rows in a loop. With Columns inside the Loop.
    // Each row looped through, 0 - 7. Off-sets the value stored in the uint32_t pixels_raw[]
    // 
    // Example:
    // pixels_raw[0]: bit0(row0), pixels_raw[1]: bit0(row0) ...
    // pixels_raw[0]: bit1(row1), pixels_raw[1]: bit1(row1) ...


    // Then.... however... 
    // Each 4bit value extracted... 
    // Need to set the value in pixels[pixel_index]
    // To, either 1=WHITE or 0=BLACK (In the color codes). 


    // The pixels. Does not have an XY Cord. Only an Index.
    // The index is idk.. lets say 300+ value.
    // When the Pixel hits pixels[48] That EQUALs NEWLINE.

    // So... 48 * (row +1)= Pixel Coordinate  (+1 so it's not 48* 0... but, depending on code, maybe I want that..)


    for (int row = 0; row < LOGO_HEIGHT; row++) {
        for (int col = 0; col < LOGO_WIDTH; col++) {    // 48 is accurate, Because 4bit per column * 12 = 48
            for (int bit = 0; bit < 8; bit++) {
                int pixel_index = (row * 8 + bit) * LOGO_WIDTH + col;       // Exmple: index.. (row (3) * 8 + bit (2) = 66) 
                //uint8_t data = (bytes << (7 - 1));



                
                //pixels[pixel_index] = (byte & (1 << (7 - 1))) ? 0xFFFFFFFF : 0xFF000000;    // if bit true, make white. else make black
            }
            //printf(" ROW: %d | COL: %d", row, col);
        }
        //printf("\n");
    }



    //// This is wrong.... 
    //// Data is only in row 0, col 0 - 48......
    // for (int row = 0; row < LOGO_HEIGHT; row++) {
    //     for (int col = 0; col < LOGO_WIDTH; col++) {
    //         uint8_t byte = logo_data[row * LOGO_WIDTH + col];               // Example. 3 * 48 + 4 (= 148)
    //         // Example Logo data [index] or .. Logo_data[148].. (0 to 383 Indexes?)

            
    //         printf(" :: |%d|%d|:%02X:", row, col, byte);

    //         // ? : is short hand for if, else.
    //         // EXAMPLE SHORTHAND: int x = (5 > 3) ? 10 : 20;  // x = 10

    //         for (int bit = 0; bit < 8; bit++) {
    //             int pixel_index = (row * 8 + bit) * LOGO_WIDTH + col;       // Exmple: index.. (row (3) * 8 + bit (2) = 66) 
    //             uint8_t data = (byte & (1 << (7 - 1)));
    //             pixels[pixel_index] = (byte & (1 << (7 - 1))) ? 0xFFFFFFFF : 0xFF000000;    // if bit true, make white. else make black
                

                
    //             // printf("Pixel data => %02X", pixels[pixel_index]);
    //         }
    //         // printf(" ROW: %d | COL: %d", row, col);
    //     }
    //     printf("\n");
    // }


    // NOt entirely sure why am updating the Width of the Logo. It's already set to 0-47..

    // Update a texture Rectangle with pixel data.  --- 
    // The last value being the Pitch: Number of bytes in a row of pixel data, (Including padding between Lines)
    SDL_UpdateTexture(texture, NULL, pixels, LOGO_WIDTH * sizeof(uint32_t)); // sizeof(uint32_t) = 4?
    
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}




int main() {
    // Startup, load rom, read logo data..
    char *rom_file = "rom/pkmn_red.gb";
    printf("Using rom file: %s\n", rom_file);

    if (load_logo_from_rom(rom_file) < 0) {
        printf("Failure reading logo from rom file..");
        return 1;
    }
    printf("Get logo..");

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



    SDL_Delay(2000);        // Display for only 2 seconds (LIkely similar to wait(2000)..)

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}