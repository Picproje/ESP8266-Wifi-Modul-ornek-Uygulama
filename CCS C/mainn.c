#include <main.h>
  #include <ctype.h>
  #include <stdio.h>
  #include <stddef.h>
  #include <stdlib.h>
  #include <stdlibm.h>
  #include <string.h>
#include "Esp8266.c"

extern char ESP8266Buf[ESP8266BUFFER_LENGHT];
/*
#int_EXT
void  EXT_isr(void) 
{

}
*/
#int_RDA
void  RDA_isr(void) 
{
   gets(ESP8266Buf);
}



void main()
{
   setup_timer_3(T3_DISABLED | T3_DIV_BY_1);

   //enable_interrupts(INT_EXT);
   enable_interrupts(INT_RDA);
   enable_interrupts(GLOBAL);

   while(TRUE)
   {
      //TODO: User Code
      ESP8266_Init();
   }

}

#use rs232(baud=115200,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8,stream=USART1)
#use rs232(baud=115200,parity=N,xmit=PIN_B1,rcv=PIN_B0,bits=8,stream=USART2)
