#include <stdint.h>
#include <stdio.h>

#define BIT_PERIOD ((90000000 * 2) / 115200)

enum State {
    IDLE,
    RECEIVING,
    STOPBIT
};

static enum State state = IDLE;
static uint8_t bits = 0;
static uint8_t byte = 0;
static uint32_t ticks = 0;
static uint32_t next_tick = 0;
static uint8_t last_val = 1;  

void printOut(uint8_t val) {
    ticks++;

    switch(state) {
        case IDLE:
            if (last_val == 1 && val == 0) {
                state = RECEIVING;
                bits = 0;
                byte = 0;
                next_tick = ticks + BIT_PERIOD + (BIT_PERIOD / 2);  
            }
            break;

        case RECEIVING:
            if (ticks == next_tick) {
                byte |= (val & 1) << bits;
                bits++;
                if (bits == 8) {
                    state = STOPBIT;
                }
                next_tick += BIT_PERIOD;
            }
            break;

        case STOPBIT:
            if (ticks == next_tick) {
                if (val == 1) {
                    printf("%c", byte);
                    fflush(stdout);
                } 
                state = IDLE;
            }
            break;
    }

    last_val = val;
}
