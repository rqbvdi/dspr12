#include <stdint.h>
#include <string.h>
#include "printf.h"
//#include "platform.h"


#define APP_START (0x00000000)
#define APP_LEN   (0x200)
#define APP_ENTRY (0x00000000)



uint32_t devP = 0xFFFFFF0;
uint16_t gpo_val = 0;


void put_Val(uint16_t value)
{
	asm ( "sh %0, 0(%1)"
           : /* no outputs */
           :"r"(value), "r"(devP) );

}

    
void main(void)
{
    //put_Val(0x155);
    //_putchar('W');
    printf("\r\nRISC-V Prozessor der\n\rHumboldt Universitaet zu Berlin\r\n");

    uint16_t led = 1;
    char c = '0';
    uint32_t counter = 0;
    while(1) {
       if(c == '~'){ c= '0';}
       put_Val(led);
       if(led == 0x8000) led = 1;
       led = led << 1;
       printf("Counter:%d - Variable 'c' as int: %d and variable 'c' as char: %c\n\r",counter++,c,c);
       c++;
    }
}
