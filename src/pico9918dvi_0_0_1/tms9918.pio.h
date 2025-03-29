// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#pragma once

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// ------- //
// tmsRead //
// ------- //

#define tmsRead_wrap_target 2
#define tmsRead_wrap 18
#define tmsRead_pio_version 0

#define tmsRead_CSR_PIN 8

static const uint16_t tmsRead_program_instructions[] = {
    0x80a0, //  0: pull   block                      
    0xa027, //  1: mov    x, osr                     
            //     .wrap_target
    0x2088, //  2: wait   1 gpio, 8                  
    0xa0e3, //  3: mov    osr, null                  
    0x6088, //  4: out    pindirs, 8                 
    0x8080, //  5: pull   noblock                    
    0xa027, //  6: mov    x, osr                     
    0xa0c3, //  7: mov    isr, null                  
    0x4001, //  8: in     pins, 1                    
    0xa046, //  9: mov    y, isr                     
    0x0085, // 10: jmp    y--, 5                     
    0x6088, // 11: out    pindirs, 8                 
    0x00cf, // 12: jmp    pin, 15                    
    0x6008, // 13: out    pins, 8                    
    0x0011, // 14: jmp    17                         
    0x6068, // 15: out    null, 8                    
    0x6008, // 16: out    pins, 8                    
    0x4003, // 17: in     pins, 3                    
    0x8020, // 18: push   block                      
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program tmsRead_program = {
    .instructions = tmsRead_program_instructions,
    .length = 19,
    .origin = -1,
    .pio_version = 0,
#if PICO_PIO_VERSION > 0
    .used_gpio_ranges = 0x1
#endif
};

static inline pio_sm_config tmsRead_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + tmsRead_wrap_target, offset + tmsRead_wrap);
    return c;
}
#endif

// -------- //
// tmsWrite //
// -------- //

#define tmsWrite_wrap_target 0
#define tmsWrite_wrap 2
#define tmsWrite_pio_version 0

#define tmsWrite_CSW_PIN 9

static const uint16_t tmsWrite_program_instructions[] = {
            //     .wrap_target
    0x2c09, //  0: wait   0 gpio, 9              [12]
    0x4010, //  1: in     pins, 16                   
    0x2c89, //  2: wait   1 gpio, 9              [12]
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program tmsWrite_program = {
    .instructions = tmsWrite_program_instructions,
    .length = 3,
    .origin = -1,
    .pio_version = 0,
#if PICO_PIO_VERSION > 0
    .used_gpio_ranges = 0x1
#endif
};

static inline pio_sm_config tmsWrite_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + tmsWrite_wrap_target, offset + tmsWrite_wrap);
    return c;
}
#endif

