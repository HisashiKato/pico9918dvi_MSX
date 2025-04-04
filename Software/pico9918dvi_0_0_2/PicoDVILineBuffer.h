// SPDX-FileCopyrightText: 2023 P Burgess for Adafruit Industries
//
// SPDX-License-Identifier: BSD-3-Clause

/*!
 * @file PicoDVI.h
 *
 * Arduino-and-Adafruit-GFX wrapper around Luke Wren's PicoDVI library.
 *
 *
 * @file PicoDVILineBuffer.h
 *
 * modified by Hisashi Kato
 *
 * https://github.com/HisashiKato/pico9918dvi_MSX
 *
 */

#pragma once

#include <PicoDVI.h>

class DVILineBuf16 {
public:
  DVILineBuf16(uint16_t w, uint16_t h, uint16_t n);
  ~DVILineBuf16(void);

  uint16_t *getLineBuffer16(void) const { return lineBuffer16; }

protected:
  uint16_t *lineBuffer16;

};

class DVILine16 : public PicoDVI, public DVILineBuf16 {
public:
  DVILine16(const DVIresolution res = DVI_RES_320x240p60,
          const struct dvi_serialiser_cfg &c = pico_sock_cfg,
          const uint16_t n_lines = 4,
          vreg_voltage v = VREG_VOLTAGE_1_20);
  ~DVILine16(void);

  bool begin(void);

  void _scanline_callback(void);

  typedef void (*dviScanlineFn)(uint16_t y, uint16_t* pixels);

  dviScanlineFn scanlineFn;

  void setScanlineFn(dviScanlineFn p) {
    scanlineFn = p;
  }

  void render_loop(void);

protected:
  uint16_t dviWidth;
  uint16_t dviHeight;

  uint16_t n_scanlines;

};





