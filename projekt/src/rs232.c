#include <stdint.h>
#include <stdio.h>

#define CLOCK_HZ        90000000     // 90 MHz
#define BAUD_RATE       115200
#define BIT_TICKS       (CLOCK_HZ / BAUD_RATE)        // 781 ticks
#define HALF_BIT_TICKS  (BIT_TICKS / 2)

static uint64_t tick = 0;
static int receiving = 0;
static uint8_t current_byte = 0;
static int bit_index = 0;
static uint64_t next_tick = 0;

void printOut(uint8_t val) {
    tick++;

    if (!receiving) {
        if (val == 0) {
            // Startbit erkannt
            receiving = 1;
            bit_index = 0;
            current_byte = 0;
            next_tick = tick + BIT_TICKS + HALF_BIT_TICKS;
        }
    } else {
        if (tick == next_tick) {
            if (bit_index < 8) {
                // Bit einlesen (LSB first)
                current_byte >>= 1;
                if (val)
                    current_byte |= 0x80;
                bit_index++;
                next_tick += BIT_TICKS;
            } else {
                // Byte vollständig → Ausgabe
                receiving = 0;
                printf("%c", current_byte);
                fflush(stdout);  // wichtig für direkte Anzeige
            }
        }
    }
}
