#include <main.h>
#include <string.h>
#include "Esp8266.c"

extern char ESP8266Buf[ESP8266BUFFER_LENGHT];

#int_RDA
void  RDA_isr(void) 
{
   gets(ESP8266Buf);
}



void main()
{
   //setup_timer_3(T3_DISABLED | T3_DIV_BY_1);

   enable_interrupts(INT_RDA);
   enable_interrupts(GLOBAL);
   //setup_oscillator(OSC_8MHZ|OSC_NORMAL|OSC_31250|OSC_PLL_ON);

   while(TRUE)
   {
      //TODO: User Code
      ESP8266_Init();
   }

}
