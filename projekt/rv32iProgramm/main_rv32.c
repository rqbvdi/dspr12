#include <stdint.h>
#include <string.h>
#include "printf.h"
#include "fptc.h"
#include "dma_test.h"

#define APP_START (0x00000000)
#define APP_LEN   (0x200)
#define APP_ENTRY (0x00000000)

fpt fpt_atan_poly_piecewise(fpt z) {
    fpt abs_z = fpt_abs(z);
    fpt z2 = fpt_mul(abs_z, abs_z);
    fpt z4 = fpt_mul(z2, z2);
    fpt z6 = fpt_mul(z4, z2);
    fpt sum;

    if (abs_z >= fl2fpt(0.5f) && abs_z <= fl2fpt(1.0f)) {
        // [0.5, 1] — Beispielkoeffizienten
        fpt a1 = fl2fpt(0.99747820f);
        fpt a3 = fl2fpt(-0.31210722f);
        fpt a5 = fl2fpt(0.13200028f);
        fpt a7 = fl2fpt(-0.03202875f);
        sum = fpt_add(a1,
                fpt_add(fpt_mul(a3, z2),
                    fpt_add(fpt_mul(a5, z4),
                            fpt_mul(a7, z6))));
    } else if (abs_z > fl2fpt(0.0f) && abs_z < fl2fpt(0.5f)) {
        // (-0.5, 0.5)
        fpt a1 = fl2fpt(0.99999267f);
        fpt a3 = fl2fpt(-0.33289221f);
        fpt a5 = fl2fpt(0.19280070f);
        fpt a7 = fl2fpt(-0.09772872f);
        sum = fpt_add(a1,
                fpt_add(fpt_mul(a3, z2),
                    fpt_add(fpt_mul(a5, z4),
                            fpt_mul(a7, z6))));
    } else if (abs_z >= fl2fpt(0.5f) && abs_z <= fl2fpt(1.0f)) {
        // [-1, -0.5], da |z| hier schon >=0.5, Vorzeichen am Ende beachten
        // Gleiche Koeffizienten wie [0.5, 1] verwenden
        fpt a1 = fl2fpt(0.99747820f);
        fpt a3 = fl2fpt(-0.31210722f);
        fpt a5 = fl2fpt(0.13200028f);
        fpt a7 = fl2fpt(-0.03202875f);
        sum = fpt_add(a1,
                fpt_add(fpt_mul(a3, z2),
                    fpt_add(fpt_mul(a5, z4),
                            fpt_mul(a7, z6))));
    } else {
        // z = 0
        return 0;
    }

    fpt result = fpt_mul(abs_z, sum);
    return z < 0 ? (-result) : result;
}

fpt fpt_atan2(fpt y, fpt x) {
    if (x == 0) {
        if (y > 0) return FPT_HALF_PI;
        if (y < 0) return -FPT_HALF_PI;
        return 0;
    }

    fpt z = fpt_div(y, x);
    fpt abs_z = fpt_abs(z);
    fpt atan;

    if (abs_z <= fl2fpt(1.0)) {
        atan = fpt_atan_poly_piecewise(z);
    } else {
        fpt inv_z = fpt_div(FPT_ONE, abs_z);
        atan = fpt_sub(FPT_HALF_PI, fpt_atan_poly_piecewise(inv_z));
        if (z < 0) atan = -atan;
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
            // Zeilenanfang mit vertikaler Linie
            if (i != 0) printf("\n");
            printf(" | ");
        }

        // Lade a0, a1, a2, a3 als raw Festkommawerte
        fpt a0 = raw2fpt(tof[i * 4 + 0]);
        fpt a1 = raw2fpt(tof[i * 4 + 1]);
        fpt a2 = raw2fpt(tof[i * 4 + 2]);
        fpt a3 = raw2fpt(tof[i * 4 + 3]);

        fpt y = fpt_sub(a3, a1);  // Zähler: a3 - a1
        fpt x = fpt_sub(a2, a0);  // Nenner: a2 - a0

        fpt angle = fpt_atan2(y, x);

       
         char out[32];
        fpt_str(angle, out, 6);  // 6 Nachkommastellen
        printf("%s", out);
        // Spaltenabschluss
        printf(" | ");
    }

    printf("\n");

    while (1);
}
