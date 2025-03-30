/*
 * Project: pico9918 - palette
 *
 * Copyright (c) 2024 Troy Schrapel
 *
 * This code is licensed under the MIT license
 *
 * https://github.com/visrealm/pico9918
 *
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif


 /* palette of 12-bit BGR values
  * Format: 0000BBBBGGGGRRRR */

extern uint16_t tms9918PaletteBGR12[16];


typedef enum {
  PICO9918PALETTE = 0, // PICO9918
  MSXPALETTE_1,        // MSX1's dull colors  wiki
  MSXPALETTE_2,        // MSX2's vivid colors
  PALETTE_NUM
} MSXcolor16;


 /* palette of 16-bit RGB565 values
  * Format: RRRRRGGGGGGBBBBB */

uint16_t rgba2rgb565(uint32_t rgbaColor);

extern uint32_t tms9918Palette[16];

void setPico9918colors (MSXcolor16 colorSet);


#ifdef __cplusplus
}
#endif

