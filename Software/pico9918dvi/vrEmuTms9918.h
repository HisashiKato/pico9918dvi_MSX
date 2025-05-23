/*
 * Troy's TMS9918 Emulator - Core interface
 *
 * Copyright (c) 2021 Troy Schrapel
 *
 * This code is licensed under the MIT license
 *
 * https://github.com/visrealm/vrEmuTms9918
 *
 */

#ifndef _VR_EMU_TMS9918_H_
#define _VR_EMU_TMS9918_H_

#define VR_EMU_TMS9918_SINGLE_INSTANCE 1



/* ------------------------------------------------------------------
 * LINKAGE MODES:
 * 
 * Default (nothing defined):    When your executable is using vrEmuTms9918 as a DLL
 * VR_6502_EMU_COMPILING_DLL:    When compiling vrEmuTms9918 as a DLL
 * VR_6502_EMU_STATIC:           When linking vrEmu6502 statically in your executable
 */

#if __EMSCRIPTEN__
#include <emscripten.h>
#ifdef __cplusplus
#define VR_EMU_TMS9918_DLLEXPORT EMSCRIPTEN_KEEPALIVE extern "C"
#define VR_EMU_TMS9918_DLLEXPORT_CONST extern "C"
#else
#define VR_EMU_TMS9918_DLLEXPORT EMSCRIPTEN_KEEPALIVE extern
#define VR_EMU_TMS9918_DLLEXPORT_CONST extern
#endif
#elif VR_TMS9918_EMU_COMPILING_DLL
#define VR_EMU_TMS9918_DLLEXPORT __declspec(dllexport)
#elif defined WIN32 && !defined VR_EMU_TMS9918_STATIC
#define VR_EMU_TMS9918_DLLEXPORT __declspec(dllimport)
#else
#ifdef __cplusplus
#define VR_EMU_TMS9918_DLLEXPORT extern "C"
#else
#define VR_EMU_TMS9918_DLLEXPORT extern
#endif
#endif

/*
#ifndef VR_EMU_TMS9918_DLLEXPORT_CONST
#define VR_EMU_TMS9918_DLLEXPORT_CONST VR_EMU_TMS9918_DLLEXPORT
#endif
*/

#if VR_EMU_TMS9918_SINGLE_INSTANCE
#define VR_EMU_INST_ARG
#define VR_EMU_INST_ONLY_ARG
#define VR_EMU_INST
#define VR_EMU_INST_ONLY
#else
#define VR_EMU_INST_ARG VrEmuTms9918* tms9918,
#define VR_EMU_INST_ONLY_ARG VrEmuTms9918* tms9918
#define VR_EMU_INST tms9918,
#define VR_EMU_INST_ONLY tms9918
#endif


#include <stdint.h>
#include <stdbool.h>

/*
#ifdef __cplusplus
extern "C" {
#endif
*/

/* -------- vrEmuTms9918Priv.h ----------- */

#define VRAM_SIZE           (1 << 14) /* 16KB */
#define VRAM_MASK     (VRAM_SIZE - 1) /* 0x3fff */

#define GRAPHICS_NUM_COLS         32
#define GRAPHICS_NUM_ROWS         24
#define GRAPHICS_CHAR_WIDTH        8

#define TEXT_NUM_COLS             40
#define TEXT_NUM_ROWS             24
#define TEXT_CHAR_WIDTH            6
#define TEXT_PADDING_PX            8
#define TEXT80_NUM_COLS           80

#define PATTERN_BYTES              8
#define GFXI_COLOR_GROUP_SIZE      8

#define MAX_SPRITES               32

#define SPRITE_ATTR_Y              0
#define SPRITE_ATTR_X              1
#define SPRITE_ATTR_NAME           2
#define SPRITE_ATTR_COLOR          3
#define SPRITE_ATTR_BYTES          4
#define LAST_SPRITE_YPOS        0xD0
#define MAX_SCANLINE_SPRITES       4

#define STATUS_INT              0x80
#define STATUS_5S               0x40
#define STATUS_COL              0x20

#define TMS_R0_MODE_TEXT_80     0x04
#define TMS_R0_MODE_GRAPHICS_II 0x02
#define TMS_R0_EXT_VDP_ENABLE   0x01

#define TMS_R1_DISP_ACTIVE      0x40
#define TMS_R1_INT_ENABLE       0x20
#define TMS_R1_MODE_MULTICOLOR  0x08
#define TMS_R1_MODE_TEXT        0x10
#define TMS_R1_SPRITE_16        0x02
#define TMS_R1_SPRITE_MAG2      0x01

#define TMS9918_PIXELS_X 256
#define TMS9918_PIXELS_Y 192

/* -------- vrEmuTms9918Priv.h end -------- */

typedef enum
{
  TMS_MODE_GRAPHICS_I,
  TMS_MODE_GRAPHICS_II,
  TMS_MODE_TEXT,
  TMS_MODE_MULTICOLOR,
  TMS_MODE_TEXT80,
} vrEmuTms9918Mode;

typedef enum
{
  TMS_TRANSPARENT = 0,
  TMS_BLACK,
  TMS_MED_GREEN,
  TMS_LT_GREEN,
  TMS_DK_BLUE,
  TMS_LT_BLUE,
  TMS_DK_RED,
  TMS_CYAN,
  TMS_MED_RED,
  TMS_LT_RED,
  TMS_DK_YELLOW,
  TMS_LT_YELLOW,
  TMS_DK_GREEN,
  TMS_MAGENTA,
  TMS_GREY,
  TMS_WHITE,
} vrEmuTms9918Color;

typedef enum
{
  TMS_REG_0 = 0,
  TMS_REG_1,
  TMS_REG_2,
  TMS_REG_3,
  TMS_REG_4,
  TMS_REG_5,
  TMS_REG_6,
  TMS_REG_7,
  TMS_NUM_REGISTERS,
  TMS_REG_NAME_TABLE        = TMS_REG_2,
  TMS_REG_COLOR_TABLE       = TMS_REG_3,
  TMS_REG_PATTERN_TABLE     = TMS_REG_4,
  TMS_REG_SPRITE_ATTR_TABLE = TMS_REG_5,
  TMS_REG_SPRITE_PATT_TABLE = TMS_REG_6,
  TMS_REG_FG_BG_COLOR       = TMS_REG_7,
} vrEmuTms9918Register;

 /* PRIVATE DATA STRUCTURE
  * ---------------------- */
struct vrEmuTMS9918_s
{
  /* the eight write-only registers */
  uint8_t registers[TMS_NUM_REGISTERS];

  /* status register (read-only) */
  uint8_t status;

  /* current address for cpu access (auto-increments) */
  uint16_t currentAddress;

  /* address or register write stage (0 or 1) */
  uint8_t regWriteStage;

  /* holds first stage of write to address/register port */
  uint8_t regWriteStage0Value;

  /* buffered value */
  uint8_t readAheadBuffer;

  bool interruptsEnabled;

  /* current display mode */
  vrEmuTms9918Mode mode;

  /* cached values */
  uint8_t spriteSize;
  bool spriteMag;
  uint16_t nameTableAddr;
  uint16_t colorTableAddr;
  uint16_t patternTableAddr;
  uint16_t spriteAttrTableAddr;
  uint16_t spritePatternTableAddr;
  vrEmuTms9918Color mainBgColor;
  vrEmuTms9918Color mainFgColor;
  bool displayEnabled;

  /* video ram */
  uint8_t vram[VRAM_SIZE];

  uint32_t rowSpriteBits[TMS9918_PIXELS_X / 32]; /* collision mask */
};

/* PRIVATE DATA STRUCTURE
 * ---------------------------------------- */
//struct vrEmuTMS9918_s;
typedef struct vrEmuTMS9918_s VrEmuTms9918;



#if VR_EMU_TMS9918_SINGLE_INSTANCE
extern VrEmuTms9918* tms9918;
#endif



/* PUBLIC INTERFACE
 * ---------------------------------------- */

#if VR_EMU_TMS9918_SINGLE_INSTANCE

 /* Function:  vrEmuTms9918Init
  * --------------------
  * initialize the TMS9918 library in single-instance mode
  */
VR_EMU_TMS9918_DLLEXPORT
void vrEmuTms9918Init();

#else

 /* Function:  vrEmuTms9918New
  * --------------------
  * create a new TMS9918
  */
VR_EMU_TMS9918_DLLEXPORT
VrEmuTms9918* vrEmuTms9918New();

#endif

/* Function:  vrEmuTms9918Reset
  * --------------------
  * reset the new TMS9918
  */
VR_EMU_TMS9918_DLLEXPORT
void vrEmuTms9918Reset(VR_EMU_INST_ONLY_ARG);

/* Function:  vrEmuTms9918Destroy
 * --------------------
 * destroy a TMS9918
 *
 * tms9918: tms9918 object to destroy / clean up
 */
VR_EMU_TMS9918_DLLEXPORT
void vrEmuTms9918Destroy(VR_EMU_INST_ONLY_ARG);

/* Function:  vrEmuTms9918WriteAddr
 * --------------------
 * write an address (mode = 1) to the tms9918
 *
 * uint8_t: the data (DB0 -> DB7) to send
 */
VR_EMU_TMS9918_DLLEXPORT
void vrEmuTms9918WriteAddr(VR_EMU_INST_ARG uint8_t data);

/* Function:  vrEmuTms9918WriteData
 * --------------------
 * write data (mode = 0) to the tms9918
 *
 * uint8_t: the data (DB0 -> DB7) to send
 */
VR_EMU_TMS9918_DLLEXPORT
void vrEmuTms9918WriteData(VR_EMU_INST_ARG uint8_t data);

/* Function:  vrEmuTms9918ReadStatus
 * --------------------
 * read from the status register
 */
VR_EMU_TMS9918_DLLEXPORT
uint8_t vrEmuTms9918ReadStatus(VR_EMU_INST_ONLY_ARG);

/* Function:  vrEmuTms9918PeekStatus
 * --------------------
 * read from the status register without resetting it
 */
VR_EMU_TMS9918_DLLEXPORT
uint8_t vrEmuTms9918PeekStatus(VR_EMU_INST_ONLY_ARG);

/* Function:  vrEmuTms9918ReadData
 * --------------------
 * read data (mode = 0) from the tms9918
 */
VR_EMU_TMS9918_DLLEXPORT
uint8_t vrEmuTms9918ReadData(VR_EMU_INST_ONLY_ARG);

/* Function:  vrEmuTms9918ReadDataNoInc
 * --------------------
 * read data (mode = 0) from the tms9918
 * don't increment the address pointer
 */
VR_EMU_TMS9918_DLLEXPORT
uint8_t vrEmuTms9918ReadDataNoInc(VR_EMU_INST_ONLY_ARG);


/* Function:  vrEmuTms9918InterruptStatus
 * --------------------
 * return true if both INT status and INT control set
 */
VR_EMU_TMS9918_DLLEXPORT
bool vrEmuTms9918InterruptStatus(VR_EMU_INST_ONLY_ARG);

/* Function:  vrEmuTms9918InterruptSet
 * --------------------
 * set interrupt flag
 */
VR_EMU_TMS9918_DLLEXPORT
void vrEmuTms9918InterruptSet(VR_EMU_INST_ONLY_ARG);

/* Function:  vrEmuTms9918SetStatus
 * --------------------
 * set status flag
 */
VR_EMU_TMS9918_DLLEXPORT
void vrEmuTms9918SetStatus(VR_EMU_INST_ARG uint8_t status);

/* Function:  vrEmuTms9918ScanLine
 * ----------------------------------------
 * generate a scanline
 *
 * pixels to be filled with TMS9918 color palette indexes (vrEmuTms9918Color)
 */
VR_EMU_TMS9918_DLLEXPORT
uint8_t vrEmuTms9918ScanLine(VR_EMU_INST_ARG uint8_t y, uint8_t pixels[TMS9918_PIXELS_X]);

/* Function:  vrEmuTms9918RegValue
 * ----------------------------------------
 * return a reigister value
 */
VR_EMU_TMS9918_DLLEXPORT
uint8_t vrEmuTms9918RegValue(VR_EMU_INST_ARG vrEmuTms9918Register reg);

/* Function:  vrEmuTms9918WriteRegValue
 * ----------------------------------------
 * write a reigister value
 */
VR_EMU_TMS9918_DLLEXPORT
void vrEmuTms9918WriteRegValue(VR_EMU_INST_ARG vrEmuTms9918Register reg, uint8_t value);


/* Function:  vrEmuTms9918VramValue
 * ----------------------------------------
 * return a value from vram
 */
VR_EMU_TMS9918_DLLEXPORT
uint8_t vrEmuTms9918VramValue(VR_EMU_INST_ARG uint16_t addr);


/* Function:  vrEmuTms9918DisplayEnabled
  * --------------------
  * check BLANK flag
  */
VR_EMU_TMS9918_DLLEXPORT
bool vrEmuTms9918DisplayEnabled(VR_EMU_INST_ONLY_ARG);


/* Function:  vrEmuTms9918DisplayMode
  * --------------------
  * current display mode
  */
VR_EMU_TMS9918_DLLEXPORT
vrEmuTms9918Mode vrEmuTms9918DisplayMode(VR_EMU_INST_ONLY_ARG);

/*
#ifdef __cplusplus
}
#endif
*/

/* Function:  vrEmuTms9918WriteAddr
 * ----------------------------------------
 * write an address (mode = 1) to the tms9918
 *
 * data: the data (DB0 -> DB7) to send
 */
inline void vrEmuTms9918WriteAddrImpl(VR_EMU_INST_ARG uint8_t data)
{
  if (tms9918->regWriteStage == 0)
  {
    /* first stage byte - either an address LSB or a register value */

    tms9918->regWriteStage0Value = data;
    tms9918->regWriteStage = 1;
  }
  else
  {
    /* second byte - either a register number or an address MSB */

    if (data & 0x80) /* register */
    {
      if ((data & 0x78) == 0)
      {
        vrEmuTms9918WriteRegValue((vrEmuTms9918Register)data, tms9918->regWriteStage0Value);
      }
    }
    else /* address */
    {
      tms9918->currentAddress = tms9918->regWriteStage0Value | ((data & 0x3f) << 8);
      if ((data & 0x40) == 0)
      {
        tms9918->readAheadBuffer = tms9918->vram[(tms9918->currentAddress++) & VRAM_MASK];
      }
    }
    tms9918->regWriteStage = 0;
  }
}

/* Function:  vrEmuTms9918ReadStatus
 * ----------------------------------------
 * read from the status register
 */
inline uint8_t vrEmuTms9918ReadStatusImpl(VR_EMU_INST_ONLY_ARG)
{
  const uint8_t tmpStatus = tms9918->status;
  tms9918->status = 0x1f;
  tms9918->regWriteStage = 0;
  return tmpStatus;
}

/* Function:  vrEmuTms9918PeekStatus
 * ----------------------------------------
 * read from the status register without resetting it
 */
inline uint8_t vrEmuTms9918PeekStatusImpl(VR_EMU_INST_ONLY_ARG)
{
  return tms9918->status;
}

/* Function:  vrEmuTms9918WriteData
 * ----------------------------------------
 * write data (mode = 0) to the tms9918
 *
 * data: the data (DB0 -> DB7) to send
 */
inline void vrEmuTms9918WriteDataImpl(VR_EMU_INST_ARG uint8_t data)
{
  tms9918->regWriteStage = 0;
  tms9918->readAheadBuffer = data;
  tms9918->vram[(tms9918->currentAddress++) & VRAM_MASK] = data;
}


/* Function:  vrEmuTms9918ReadData
 * ----------------------------------------
 * read data (mode = 0) from the tms9918
 */
inline uint8_t vrEmuTms9918ReadDataImpl(VR_EMU_INST_ONLY_ARG)
{
  tms9918->regWriteStage = 0;
  uint8_t currentValue = tms9918->readAheadBuffer;
  tms9918->readAheadBuffer = tms9918->vram[(tms9918->currentAddress++) & VRAM_MASK];
  return currentValue;
}

/* Function:  vrEmuTms9918ReadDataNoInc
 * ----------------------------------------
 * read data (mode = 0) from the tms9918
 */
inline uint8_t vrEmuTms9918ReadDataNoIncImpl(VR_EMU_INST_ONLY_ARG)
{
  return tms9918->readAheadBuffer;
}

/* Function:  vrEmuTms9918InterruptStatus
 * --------------------
 * return true if both INT status and INT control set
 */
inline bool vrEmuTms9918InterruptStatusImpl(VR_EMU_INST_ONLY_ARG)
{
  return tms9918->interruptsEnabled && (tms9918->status & STATUS_INT);
}

/* Function:  vrEmuTms9918InterruptSet
 * --------------------
 * return true if both INT status and INT control set
 */
inline void vrEmuTms9918InterruptSetImpl(VR_EMU_INST_ONLY_ARG)
{
  tms9918->status |= STATUS_INT;
}

/* Function:  vrEmuTms9918SetStatus
 * --------------------
 * set status flag
 */
inline
void vrEmuTms9918SetStatusImpl(VR_EMU_INST_ARG uint8_t status)
{
  tms9918->status = status;
}

#endif // _VR_EMU_TMS9918_H_



