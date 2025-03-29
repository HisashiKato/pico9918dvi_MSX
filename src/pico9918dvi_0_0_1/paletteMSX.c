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

#include "pico/stdlib.h"
#include "paletteMSX.h"

#define TMSPAL00 0x00000000 /* transparent*/
#define TMSPAL01 0x000000ff /* black */
#define TMSPAL02 0x21c942ff /* medium green */
#define TMSPAL03 0x5edc78ff /* light green */
#define TMSPAL04 0x5455edff /* dark blue */
#define TMSPAL05 0x7d75fcff /* light blue */
#define TMSPAL06 0xd3524dff /* dark red */
#define TMSPAL07 0x43ebf6ff /* cyan */
#define TMSPAL08 0xfd5554ff /* medium red */
#define TMSPAL09 0xff7978ff /* light red */
#define TMSPAL10 0xd3c153ff /* dark yellow */
#define TMSPAL11 0xe5ce80ff /* light yellow */
#define TMSPAL12 0x21b03cff /* dark green */
#define TMSPAL13 0xc95bbaff /* magenta */
#define TMSPAL14 0xccccccff /* grey */
#define TMSPAL15 0xffffffff /* white */


 /* macros to convert from RGBA32 to BGR12 */
#define RFROMRGBA(rgba) ((rgba >> 24) & 0xff)
#define GFROMRGBA(rgba) ((rgba >> 16) & 0xff)
#define BFROMRGBA(rgba) ((rgba >> 8) & 0xff)
#define C4FROMC8(c4) ((uint16_t)(c4 / 16.0f) & 0x0f)
#define BGR12FROMRGB(r, g, b) ((C4FROMC8(b) << 8) | (C4FROMC8(g) << 4) | C4FROMC8(r))
#define BGR12FROMRGBA32(rgba32) (BGR12FROMRGB(RFROMRGBA(rgba32), GFROMRGBA(rgba32), BFROMRGBA(rgba32)))

 /* palette of 12-bit BGR values generated from above 32-bit RGBA values
  * Format: 0000BBBBGGGGRRRR */

uint16_t __aligned(4) tms9918PaletteBGR12[16] =
{
  BGR12FROMRGBA32(TMSPAL00),
  BGR12FROMRGBA32(TMSPAL01),
  BGR12FROMRGBA32(TMSPAL02),
  BGR12FROMRGBA32(TMSPAL03),
  BGR12FROMRGBA32(TMSPAL04),
  BGR12FROMRGBA32(TMSPAL05),
  BGR12FROMRGBA32(TMSPAL06),
  BGR12FROMRGBA32(TMSPAL07),
  BGR12FROMRGBA32(TMSPAL08),
  BGR12FROMRGBA32(TMSPAL09),
  BGR12FROMRGBA32(TMSPAL10),
  BGR12FROMRGBA32(TMSPAL11),
  BGR12FROMRGBA32(TMSPAL12),
  BGR12FROMRGBA32(TMSPAL13),
  BGR12FROMRGBA32(TMSPAL14),
  BGR12FROMRGBA32(TMSPAL15),
};




uint16_t rgba2rgb565(uint32_t rgbaColor) {

    uint8_t r,g,b;
    uint16_t c565;

    r = (uint8_t)((rgbaColor & 0xFF000000) >> 24);
    g = (uint8_t)((rgbaColor & 0x00FF0000) >> 16);
    b = (uint8_t)((rgbaColor & 0x0000FF00) >> 8);
    c565 = (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));

    return c565;
}


const uint32_t vrEmuTms9918Palette0[3][16] = {
  {             // PICO9918 default
    0x00000000, /* transparent */
    0x000000ff, /* black */
    0x21c942ff, /* medium green */
    0x5edc78ff, /* light green */
    0x5455edff, /* dark blue */
    0x7d75fcff, /* light blue */
    0xd3524dff, /* dark red */
    0x43ebf6ff, /* cyan */
    0xfd5554ff, /* medium red */
    0xff7978ff, /* light red */
    0xd3c153ff, /* dark yellow */
    0xe5ce80ff, /* light yellow */
    0x21b03cff, /* dark green */
    0xc95bbaff, /* magenta */
    0xccccccff, /* grey */
    0xffffffff  /* white */
  },

  {
    0x00000000, /* transparent */
    0x000000ff, /* black */
    0x3eb849ff, /* medium green */ // 62,184, 73
    0x74d07dff, /* light green */  //116,208,125
    0x5955e0ff, /* dark blue */    // 89, 85,224
    0x8076f1ff, /* light blue */   //128,118,241
    0xb95e51ff, /* dark red */     //185, 94, 81
    0x65dbefff, /* cyan */         //101,219,239
    0xdb6559ff, /* medium red */   //219,101, 89
    0xff897dff, /* light red */    //255,137,125
    0xccc35eff, /* dark yellow */  //204,195, 94
    0xded087ff, /* light yellow */ //222,208,135
    0x3aa241ff, /* dark green */   // 58,162, 65
    0xb766b5ff, /* magenta */      //183,102,181
    0xccccccff, /* grey */         //204,204,204
    0xffffffff  /* white */        //255,255,255
  },

  {
    0x00000000, /* transparent */
    0x000000ff, /* black */
    0x22dd22ff, /* medium green */
    0x66ff66ff, /* light green */
    0x2222ffff, /* dark blue */
    0x4466ffff, /* light blue */
    0xaa2222ff, /* dark red */
    0x44ddffff, /* cyan */
    0xff2222ff, /* medium red */
    0xff6666ff, /* light red */
    0xdddd22ff, /* dark yellow */
    0xdddd88ff, /* light yellow */
    0x228822ff, /* dark green */
    0xdd44aaff, /* magenta */
    0xaaaaaaff, /* grey */
    0xffffffff  /* white */
  }
};

void setPico9918colors (MSXcolor16 colorSet) {
    for (int i = 0; i < 16; i++) {
        tms9918Palette[i] = vrEmuTms9918Palette0[colorSet][i];
    }
}

