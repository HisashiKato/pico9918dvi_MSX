/*
 * Project: pico9918
 *
 * Copyright (c) 2024 Troy Schrapel
 *
 * This code is licensed under the MIT license
 *
 * https://github.com/visrealm/pico9918
 *
 *
 * modified by Hisashi Kato
 *
 * https://github.com/HisashiKato/pico9918dvi_MSX
 *
 */


#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/binary_info.h"

#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/vreg.h"


#include "clocks.pio.h"
#include "tms9918.pio.h"

#include "paletteMSX.h"

#include "vrEmuTms9918.h"
#include "vrEmuTms9918Util.h"

/*
  * Pin mapping (RP2350)
  *
  * Pin  | GPIO | Name      | TMS9918A Pin
  * -----+------+-----------+-------------
  *   1  |   0  |  CD7      |  24
  *   2  |   1  |  CD6      |  23
  *   4  |   2  |  CD5      |  22
  *   5  |   3  |  CD4      |  21
  *   6  |   4  |  CD3      |  20
  *   7  |   5  |  CD2      |  19
  *   9  |   6  |  CD1      |  18
  *  10  |   7  |  CD0      |  17
  *  11  |   8  |  /CSR     |  15
  *  12  |   9  |  /CSW     |  14
  *  14  |  10  |  MODE     |  13
  *  30  |  RUN |  RST      |  34
  *  31  |  26  |  /INT     |  16
  *  32  |  27  |  CPUCLK   |  38
  *   x  |   x  |  GROMCLK  |  37 // MSX1 is not Use GROMCLK
*/


#define PCB_MAJOR_VERSION 0
#define PCB_MINOR_VERSION 3

/* The data bus of TMS9918 is  (MSB)CD0 - CD7(LSB) */
#define GPIO_CD7 0 // CD 7-0 GPIO 0-7
#define GPIO_CSR tmsRead_CSR_PIN  // defined in tms9918.pio // GPIO 8
#define GPIO_CSW tmsWrite_CSW_PIN // defined in tms9918.pio // GPIO 9
#define GPIO_MODE 10 // MODE GPIO 10
#define GPIO_INT 26  // INT  GPIO 26

#if PCB_MAJOR_VERSION != 0
#error "Time traveller?"
#endif

#if PCB_MINOR_VERSION < 4
#define GPIO_CPUCL 27 // CPUCLK GPIO 27
#else
#define GPIO_GROMCL 25
#define GPIO_CPUCL 24
#define GPIO_RESET 23
#define GPIO_MODE1 29
#endif


#define GPIO_CD_MASK (0xff << GPIO_CD7)
#define GPIO_CSR_MASK (0x01 << GPIO_CSR)
#define GPIO_CSW_MASK (0x01 << GPIO_CSW)
#define GPIO_MODE_MASK (0x01 << GPIO_MODE)
#define GPIO_INT_MASK (0x01 << GPIO_INT)

#define TMS_CRYSTAL_FREQ_HZ 10738635.0f

// for 31.46875 kHz
#define PICO_CLOCK_PLL 1260000000
#define PICO_CLOCK_PLL_DIV1 5
#define PICO_CLOCK_PLL_DIV2 1

#define PICO_CLOCK_HZ (PICO_CLOCK_PLL / PICO_CLOCK_PLL_DIV1 / PICO_CLOCK_PLL_DIV2)

#define TMS_PIO pio1
#define TMS_IRQ PIO1_IRQ_0

#define CLK_PIO pio0

static uint8_t nextValue = 0;     /* TMS9918A read-ahead value */
static bool currentInt = false;   /* current interrupt state */
static uint8_t currentStatus = 0x1f; /* current status register value */

const uint tmsWriteSm = 0;
const uint tmsReadSm = 1;


/*
 * update the value send to the read PIO
 */
inline static void updateTmsReadAhead() {
    uint32_t readAhead = 0xff;              // pin direction
    readAhead |= nextValue << 8;
    readAhead |= currentStatus << 16;
    pio_sm_put(TMS_PIO, tmsReadSm, readAhead);
}


/*
 * handle interrupts from the TMS9918<->CPU interface
 */
void  __not_in_flash_func(pio_irq_handler)() {

    if ((TMS_PIO->fstat & (1u << (PIO_FSTAT_RXEMPTY_LSB + tmsWriteSm))) == 0) { // write?

        uint32_t writeVal = TMS_PIO->rxf[tmsWriteSm];

        if (writeVal & (GPIO_MODE_MASK >> GPIO_CD7)) { // write reg/addr
            vrEmuTms9918WriteAddrImpl(writeVal & 0xff);
            currentInt = vrEmuTms9918InterruptStatusImpl();
            gpio_put(GPIO_INT, !currentInt);
        } else { // write data
            vrEmuTms9918WriteDataImpl(writeVal & 0xff);
        }
        nextValue = vrEmuTms9918ReadDataNoIncImpl();
        updateTmsReadAhead();

    } else if ((TMS_PIO->fstat & (1u << (PIO_FSTAT_RXEMPTY_LSB + tmsReadSm))) == 0) { // read?

        uint32_t readVal = TMS_PIO->rxf[tmsReadSm];

        if ((readVal & 0x04) == 0) { // read data
            vrEmuTms9918ReadDataImpl();
            nextValue = vrEmuTms9918ReadDataNoIncImpl();
        } else { // read status
            currentStatus = 0x1f;
            vrEmuTms9918SetStatusImpl(currentStatus);
            currentInt = false;
            gpio_put(GPIO_INT, !currentInt);
        }
        updateTmsReadAhead();
    }
}


/*
 * enable gpio interrupts inline
 */
static inline void enableTmsPioInterrupts() {
    __dmb();
#if PICO_RP2040
    *((io_rw_32*)(PPB_BASE + M0PLUS_NVIC_ICPR_OFFSET)) = 1u << TMS_IRQ;
    *((io_rw_32*)(PPB_BASE + M0PLUS_NVIC_ISER_OFFSET)) = 1u << TMS_IRQ;
#else
    *((io_rw_32*)(PPB_BASE + M33_NVIC_ICPR0_OFFSET)) = 1u << TMS_IRQ;
    *((io_rw_32*)(PPB_BASE + M33_NVIC_ISER0_OFFSET)) = 1u << TMS_IRQ;
#endif
}

/*
 * disable gpio interrupts inline
 */
static inline void disableTmsPioInterrupts() {
#if PICO_RP2040
    *((io_rw_32*)(PPB_BASE + M0PLUS_NVIC_ICER_OFFSET)) = 1u << TMS_IRQ;
#else
    *((io_rw_32*)(PPB_BASE + M33_NVIC_ICER0_OFFSET)) = 1u << TMS_IRQ;
#endif
    __dmb();
}


/*
 * initialise a clock output using PIO
 */
uint initClock(uint gpio, float freqHz) {
  //static uint clocksPioOffset = -1;
    static int clocksPioOffset = -1;

    if (clocksPioOffset == -1) {
        clocksPioOffset = pio_add_program(CLK_PIO, &clock_program);
    }

  //static uint clkSm = 2;
    static uint clkSm = 3; // PicoDVI Library use sm 0,1,2 

    pio_gpio_init(CLK_PIO, gpio);
    pio_sm_set_consecutive_pindirs(CLK_PIO, clkSm, gpio, 1, true);
    pio_sm_config c = clock_program_get_default_config((uint)clocksPioOffset);
    sm_config_set_set_pins(&c, gpio, 1);

    pio_sm_init(CLK_PIO, clkSm, clocksPioOffset, &c);

    float clockDiv = (float)PICO_CLOCK_HZ / (freqHz * 2.0f);
    pio_sm_set_clkdiv(CLK_PIO, clkSm, clockDiv);
    pio_sm_set_enabled(CLK_PIO, clkSm, true);

  //return clkSm++;
    return 0;
}


/*
 * Set up PIOs for TMS9918 <-> CPU interface
 */
void tmsPioInit() {
    irq_set_exclusive_handler(TMS_IRQ, pio_irq_handler);
    irq_set_enabled(TMS_IRQ, true);

    uint tmsWriteProgram = pio_add_program(TMS_PIO, &tmsWrite_program);

    pio_sm_config writeConfig = tmsWrite_program_get_default_config(tmsWriteProgram);
    sm_config_set_in_pins(&writeConfig, GPIO_CD7);
    sm_config_set_in_shift(&writeConfig, false, true, 16); // L shift, autopush @ 16 bits
    sm_config_set_clkdiv(&writeConfig, 1.0f);

    pio_sm_init(TMS_PIO, tmsWriteSm, tmsWriteProgram, &writeConfig);
    pio_sm_set_enabled(TMS_PIO, tmsWriteSm, true);
    pio_set_irq0_source_enabled(TMS_PIO, pis_sm0_rx_fifo_not_empty, true);

    uint tmsReadProgram = pio_add_program(TMS_PIO, &tmsRead_program);

    for (uint i = 0; i < 8; ++i) {
        pio_gpio_init(TMS_PIO, GPIO_CD7 + i);
    }

    pio_sm_config readConfig = tmsRead_program_get_default_config(tmsReadProgram);
    sm_config_set_in_pins(&readConfig, GPIO_CSR);
    sm_config_set_jmp_pin(&readConfig, GPIO_MODE);
    sm_config_set_out_pins(&readConfig, GPIO_CD7, 8);
    sm_config_set_in_shift(&readConfig, false, false, 32); // L shift
    sm_config_set_out_shift(&readConfig, true, false, 32); // R shift
    sm_config_set_clkdiv(&readConfig, 4.0f);

    pio_sm_init(TMS_PIO, tmsReadSm, tmsReadProgram, &readConfig);
    pio_sm_set_enabled(TMS_PIO, tmsReadSm, true);
    pio_set_irq0_source_enabled(TMS_PIO, pis_sm1_rx_fifo_not_empty, true);

    pio_sm_put(TMS_PIO, tmsReadSm, 0x000000ff);
}



//#include <PicoDVI.h>
#include "PicoDVILineBuffer.h"

#define DVI_SCREEN_WIDTH  320
#define DVI_SCREEN_HEIGHT 240

uint32_t tms9918Palette[16];

const uint32_t vBorder = ((DVI_SCREEN_HEIGHT - TMS9918_PIXELS_Y) / 2);
const uint32_t hBorder = ((DVI_SCREEN_WIDTH - TMS9918_PIXELS_X) / 2);

static uint8_t __aligned(4) tmsScanlineBuffer[TMS9918_PIXELS_X];

DVILine16 display(DVI_RES_320x240p60, pico_sock_cfg);

 
//static void __time_critical_func(tmsScanline)(uint16_t y, uint8_t* pixels){
static void __time_critical_func(tmsScanline)(uint16_t y, uint16_t* pixels){

    uint16_t bgColor = rgba2rgb565(tms9918Palette[vrEmuTms9918RegValue(TMS_REG_FG_BG_COLOR) & 0x0f]);

    /*** top and bottom borders ***/
    if ((y < vBorder) || (y >= (vBorder + TMS9918_PIXELS_Y))) {
        for (uint x = 0; x < DVI_SCREEN_WIDTH; x++) {
            pixels[x] = bgColor;
        }
        return;
    }

    /*** main display region ***/
    y -= vBorder;
    /* generate the scanline */
    uint8_t tempStatus = vrEmuTms9918ScanLine(y, tmsScanlineBuffer);

    // ***interrupt signal? ***
    if (y == TMS9918_PIXELS_Y - 1){
        tempStatus |= STATUS_INT;
    }

    disableTmsPioInterrupts();
    if ((currentStatus & STATUS_INT) == 0){
        currentStatus = (currentStatus & 0xe0) | tempStatus;
        vrEmuTms9918SetStatusImpl(currentStatus);
        updateTmsReadAhead();
        currentInt = vrEmuTms9918InterruptStatusImpl();
        gpio_put(GPIO_INT, !currentInt);
    }
    enableTmsPioInterrupts();

    /*** left border ***/
    for (uint x = 0; x < hBorder; ++x) {
        pixels[x] = bgColor;
    }

    uint tmsX = 0;
    for (uint x = hBorder; x < hBorder + TMS9918_PIXELS_X; ++x, ++tmsX) {
        pixels[x] = rgba2rgb565(tms9918Palette[tmsScanlineBuffer[tmsX] & 0x0f]);
    }

    /*** right border ***/
    for (uint x = hBorder + TMS9918_PIXELS_X; x < DVI_SCREEN_WIDTH; ++x) {
        pixels[x] = bgColor;
    }
}



void setup() {
  // put your setup code here, to run once:

  //set_sys_clock_pll(PICO_CLOCK_PLL, PICO_CLOCK_PLL_DIV1, PICO_CLOCK_PLL_DIV2);   // 252000

    /* we need one of these. it's the main guy */
    vrEmuTms9918Init();

    // set up gpio pins
    gpio_init_mask(GPIO_CD_MASK | GPIO_CSR_MASK | GPIO_CSW_MASK | GPIO_MODE_MASK | GPIO_INT_MASK);
    gpio_put_all(GPIO_INT_MASK);
    gpio_set_dir_all_bits(GPIO_INT_MASK); // int is an output

    tmsPioInit();

    // set up the CPUCLK signals
    initClock(GPIO_CPUCL, TMS_CRYSTAL_FREQ_HZ / 3.0f);

    /*        
      The 16 display colors can be changed by defining your 
      preferred color in "paletteMSX.c"
      For example, the vivid colors seen when running
      MSX1 software on MSX2

      PICO9918PALETTE //PICO9918 default colors
      MSXPALETTE_1    //MSX1's dull colors
      MSXPALETTE_2    //MSX2's vivid colors
    */
    setPico9918colors(PICO9918PALETTE);

    display.setScanlineFn(&tmsScanline);
    display.begin();
    display.render_loop();
}


void loop() {
  // put your main code here, to run repeatedly:

}


