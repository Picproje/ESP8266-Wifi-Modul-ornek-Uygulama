#include <18F4550.h>
#device adc=16

#FUSES NOWDT                    //No Watch Dog Timet
#FUSES CPUDIV4                  //System Clock by 4
#FUSES XT                       //Crystal osc <= 4mhz for PCM/PCH , 3mhz to 10 mhz for PCD
#FUSES NOFCMEN                  //Fail-safe clock monitor disabled
#FUSES NOBROWNOUT               //No brownout reset
#FUSES NOPBADEN                 //PORTB pins are configured as digital I/O on RESET
#FUSES NOLPT1OSC                //Timer1 configured for higher power operation
#FUSES NOSTVREN                 //Stack full/underflow will not cause reset
#FUSES NOLVP                    //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O
#FUSES NOXINST                  //Extended set extension and Indexed Addressing mode disabled (Legacy mode)

#use delay(crystal=20000000,  clock=5000000)
#priority rda,ext


#use fixed_IO(C_outputs = PIN_C6,PIN_C7)
#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8,stream= Module)
#use rs232(baud=9600,parity=N,xmit=PIN_B1,rcv=PIN_B0,bits=8,stream= PC)     //Yazýlýmsal olanýn pinlerini rastgele yaptým sen seç kafana göre

