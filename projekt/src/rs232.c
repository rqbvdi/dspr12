#include <stdint.h>
#include <stdio.h>

#define BAUD_RATE 115200
#define CLOCK_FREQ 90000000
#define SIM_TICKS_PER_SEC (CLOCK_FREQ * 2)  // beide Flanken
#define BIT_PERIOD (SIM_TICKS_PER_SEC / BAUD_RATE)  // ≈ 1562

// Empfangsstatus
enum State {
    IDLE,
    RECEIVING,
    STOPBIT
};

static enum State state = IDLE;
static uint8_t bit_count = 0;
static uint8_t data_byte = 0;
static uint32_t tick_counter = 0;
static uint32_t next_sample_tick = 0;
static uint8_t last_val = 1;  // idle state ist '1'

void printOut(uint8_t val) {
    tick_counter++;

    switch(state) {
        case IDLE:
            if (last_val == 1 && val == 0) {
                // Startbit erkannt
                state = RECEIVING;
                bit_count = 0;
                data_byte = 0;
                next_sample_tick = tick_counter + BIT_PERIOD + (BIT_PERIOD / 2);  // Mitte erstes Bit
            }
            break;

        case RECEIVING:
            if (tick_counter == next_sample_tick) {
                // Datenbit empfangen (LSB first)
                data_byte |= (val & 1) << bit_count;
                bit_count++;
                if (bit_count == 8) {
                    state = STOPBIT;
                }
                next_sample_tick += BIT_PERIOD;
            }
            break;

        case STOPBIT:
            if (tick_counter == next_sample_tick) {
                if (val == 1) {
                    // Zeichen vollständig empfangen
                    printf("%c", data_byte);
                    fflush(stdout);
                } else {
                    // Fehler: Kein korrektes Stopbit
                    printf("Fehler: Kein korrektes Stopbit nach %c (%d)\n", data_byte, data_byte);
                    fflush(stdout);
                }
                state = IDLE;
            }
            break;
    }

    last_val = val;
}
