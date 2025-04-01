// SPDX-FileCopyrightText: 2023 P Burgess for Adafruit Industries
//
// SPDX-License-Identifier: BSD-3-Clause

/*!
 * @file PicoDVI.cpp
 *
 * Arduino-and-Adafruit-GFX wrapper around Luke Wren's PicoDVI library.
 *
 *
 * @file PicoDVILineBuffer.cpp
 *
 * modified by Hisashi Kato
 *
 * https://github.com/HisashiKato/pico9918dvi_MSX
 *
 */

#include "PicoDVILineBuffer.h"
#include <PicoDVI.h>


static struct {
  const dvi_timing &timing;
  vreg_voltage v;
  uint16_t width;
  uint16_t height;
  uint8_t v_rep;
} _dvispec[] = {
    {dvi_timing_640x480p_60hz, VREG_VOLTAGE_1_20, 320, 240, 2},
    {dvi_timing_720x480p_60hz, VREG_VOLTAGE_1_20, 360, 240, 2},
    {dvi_timing_800x480p_60hz, VREG_VOLTAGE_1_20, 400, 240, 2},
    {dvi_timing_800x480p_30hz, VREG_VOLTAGE_1_20, 400, 240, 2},
    {dvi_timing_640x480p_60hz, VREG_VOLTAGE_1_20, 640, 480, 1},
    {dvi_timing_720x480p_60hz, VREG_VOLTAGE_1_20, 720, 480, 1},
    {dvi_timing_800x480p_60hz, VREG_VOLTAGE_1_20, 800, 480, 1},
    {dvi_timing_800x480p_30hz, VREG_VOLTAGE_1_20, 800, 480, 1},
    {dvi_timing_640x480p_60hz, VREG_VOLTAGE_1_20, 640, 240, 2},
    {dvi_timing_720x480p_60hz, VREG_VOLTAGE_1_20, 720, 240, 2},
    {dvi_timing_800x480p_60hz, VREG_VOLTAGE_1_20, 800, 240, 2},
    {dvi_timing_800x480p_30hz, VREG_VOLTAGE_1_20, 800, 240, 2},
    // Additional resolutions might get added here if the overclock issue can
    // be sorted out. Regardless, always keep this list 1:1 in sync with the
    // DVIresolution enum in PicoDVI.h.
    {dvi_timing_1280x720p_30hz, VREG_VOLTAGE_1_30, 1280, 720, 1},
};



DVILineBuf16::DVILineBuf16(uint16_t w, uint16_t h, uint16_t n) {
  uint32_t mbytes;
  mbytes = w * 2 * n;
  if ((lineBuffer16 = (uint16_t *)malloc(mbytes))) {
    memset(lineBuffer16, 0, mbytes);
  }
}


DVILineBuf16::~DVILineBuf16(void) {
  if (lineBuffer16)
    free(lineBuffer16);
}


static void *linefnptr = NULL; // For C access to active C++ object

DVILine16::DVILine16(const DVIresolution r, const struct dvi_serialiser_cfg &c, const uint16_t n_lines, vreg_voltage v)
    : PicoDVI(_dvispec[r].timing, c, v), 
      DVILineBuf16(_dvispec[r].width, _dvispec[r].height, n_lines) {
  dviWidth = _dvispec[r].width;
  dviHeight = _dvispec[r].height;
  n_scanlines = n_lines;  
  dvi_vertical_repeat = _dvispec[r].v_rep;
  dvi_monochrome_tmds = false;
}

DVILine16::~DVILine16(void) {
  linefnptr = NULL;
}

static void __not_in_flash_func(scanline_callback_GFXLine16)(void) {
  ((DVILine16 *)linefnptr)->_scanline_callback();
}

void __not_in_flash_func(DVILine16::_scanline_callback)(void) {

}


void __not_in_flash_func(DVILine16::render_loop)(void) {
  while (1) {
    for (uint16_t scanline = 0; scanline < dviHeight; ++scanline) {
      uint16_t *pixbuf;
      queue_remove_blocking_u32(&dvi0.q_colour_free, &pixbuf);
      scanlineFn(scanline, pixbuf);
      queue_add_blocking_u32(&dvi0.q_colour_valid, &pixbuf);
    }
  }
}


bool DVILine16::begin(void) {
  uint16_t *linebufptr16 = getLineBuffer16();
  if (linebufptr16) {
    linefnptr = this;

    mainloop = dvi_scanbuf_main_16bpp; // in libdvi
    dvi0.scanline_callback = scanline_callback_GFXLine16;
    PicoDVI::begin();

    for (int i = 0; i < n_scanlines; ++i) {
      void *bufptr = (void *)(linebufptr16 + (dviWidth * i));
      queue_add_blocking_u32(&dvi0.q_colour_free, &bufptr);
    }

    wait_begin = false; // Set core 1 in motion

    return true;
  }
  return false;
}


