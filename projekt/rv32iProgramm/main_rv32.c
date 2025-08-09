#include <stdint.h>
#include <string.h>
#include "printf.h"
#include "fptc.h"
#include "dma_test.h"

#define APP_START (0x00000000)
#define APP_LEN   (0x200)
#define APP_ENTRY (0x00000000)

fpt fpt_atan_poly_5th(fpt z) {
    fpt z2 = fpt_mul(z, z);       // z^2
    fpt z3 = fpt_mul(z2, z);      // z^3
    fpt z5 = fpt_mul(z3, z2);     // z^5

    // Koeffizienten bitte aus Python-Code einsetzen
    fpt a1 = fl2fpt(0.99596531f);        // Platzhalter
    fpt a3 = fl2fpt(-0.29217759f);    // Platzhalter
    fpt a5 = fl2fpt(0.08290755f);        // Platzhalter

    fpt term1 = fpt_mul(a1, z);
    fpt term3 = fpt_mul(a3, z3);
    fpt term5 = fpt_mul(a5, z5);

    return fpt_add(fpt_add(term1, term3), term5);
}

// Beispiel für fpt_atan2 - ähnlich wie du hattest, angepasst auf neue poly-Funktion
fpt fpt_atan2(fpt y, fpt x) {
    if (x == 0) {
        if (y > 0) return FPT_HALF_PI;
        if (y < 0) return -FPT_HALF_PI;
        return 0;
    }

    fpt z = fpt_div(y, x);
    fpt abs_z = fpt_abs(z);
    fpt atan;

    if (abs_z <= fl2fpt(1.0f)) {
        atan = fpt_atan_poly_5th(z);
    } else {
        fpt inv_z = fpt_div(FPT_ONE, abs_z);
        atan = fpt_sub(FPT_HALF_PI, fpt_atan_poly_5th(inv_z));
        if (z < 0) atan = (-atan);
    }

    if (x < 0) {
        if (y < 0) return fpt_sub(atan, FPT_PI);
        else       return fpt_add(atan, FPT_PI);
    }
    return atan;
}


void main(void) {
    printf("\r\nRISC-V Prozessor der\n\rHumboldt Universitaet zu Berlin\r\n");

    for (int i = 0; i < 64; ++i) {
        if (i % 8 == 0) {
            if (i != 0) printf("\n");
            printf(" | ");
        }

        fpt a0 = raw2fpt(tof[i * 4 + 0]);
        fpt a1 = raw2fpt(tof[i * 4 + 1]);
        fpt a2 = raw2fpt(tof[i * 4 + 2]);
        fpt a3 = raw2fpt(tof[i * 4 + 3]);

        fpt y = fpt_sub(a3, a1);  
        fpt x = fpt_sub(a2, a0);  

        fpt angle = fpt_atan2(y, x);

       
        char out[32];
        fpt_str(angle, out, 6);  
        printf("%s", out);
        printf(" | ");
    }

    printf("\n");

    while (1);
}
