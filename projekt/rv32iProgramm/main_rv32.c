#include <stdint.h>
#include <string.h>
#include "printf.h"
#include "fptc.h"
#include "dma_test.h"

#define APP_START (0x00000000)
#define APP_LEN   (0x200)
#define APP_ENTRY (0x00000000)

fpt helper(fpt z) {
    fpt z2 = fpt_mul(z, z);     
    fpt z3 = fpt_mul(z2, z);     
    fpt z5 = fpt_mul(z3, z2);
    fpt c1 = fl2fpt(0.99596531f);        
    fpt c3 = fl2fpt(-0.29217759f);
    fpt c5 = fl2fpt(0.08290755f);        

    fpt pol1 = fpt_mul(c1, z);
    fpt pol3 = fpt_mul(c3, z3);
    fpt pol5 = fpt_mul(c5, z5);

    return fpt_add(fpt_add(pol1, pol3), pol5);
}
fpt fpt_atan2(fpt y, fpt x) {
    if (x == 0) {
        return (y > 0) ? FPT_HALF_PI : (y < 0 ? -FPT_HALF_PI : 0);
    }

    fpt z = fpt_div(y, x);
    fpt abs_z = fpt_abs(z);
    fpt atan;

    if (abs_z <= fl2fpt(1.0f)) {
        atan = helper(z);
    } else {
        fpt inv_z = fpt_div(FPT_ONE, abs_z);
        atan = fpt_sub(FPT_HALF_PI, helper(inv_z));
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

    for (int i = 0; i < 64; i++) {
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
