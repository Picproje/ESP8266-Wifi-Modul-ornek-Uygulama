#include "Typedefs.h"

#define ESP8266BUFFER_LENGHT 500

char Network_User_Pass[] = "\"MamiDroid\",\"sakaryali54\"";

char ESP8266Buf[ESP8266BUFFER_LENGHT];
static uint16_t ESPWriteIndex=0;

/***********************************************************
* Function Name  : Clear_ESPBuffer
* Description    : Buffer temizlenir.
* Input          : None
* Return         : None
***********************************************************/
static void Clear_ESPBuffer(void)
{
    uint16_t i;

    for(i=0;i<ESP8266BUFFER_LENGHT;i++)
        ESP8266Buf[i] = 0;

    ESPWriteIndex=0;
}

/***********************************************************
* Function Name  : ESP8266_Init
* Description    : Modül bir aga baglanana kadar burada kalir.
* Input          : None
* Return         : None
***********************************************************/
static void ESP8266_Init(void)
{
    static uint8_t ESPInitCase=0;
    char TempStr[10];
    // Modul Reset Pini High yapiliyor.
    output_high(pin_b2);

    switch(ESPInitCase)
    {
        case 0:
            // Standart AT Komutu gonderilir , Cevabi OK olmalidir.
            fputs("AT\r\n",USART1);
            // 1 saniye gecikme koyuyoruz.
            delay_ms(1000);
            ESPInitCase = 1;
        break;

        case 1:
            // STRSTR fonksiyonu metin icinde metin arayan bir fonksiyondur
            // Eger aranilan metni bulabilirse pointer adresi ile geri donus yapar
            // Bulamaz ise NULL dondurur. Bizde null'dan farklý mý diye sorariz.
            strcpy(TempStr,"OK");
            if (strstr(ESP8266Buf,TempStr) != NULL)
            {
                Clear_ESPBuffer();
                fputs("Module Erisildi",USART2);
                ESPInitCase = 2;
            }
            else
            {
                Clear_ESPBuffer();
                fputs("Modul Bulunamadi, Tekrar Deneniyor",USART2);
                ESPInitCase = 0;
            }
        break;

        case 2:
            // Modulun 3 modu var gerekli bilgi datasheet'de biz 1 olmasini istiyoruz
            fputs("AT+CWMODE?\r\n",USART1);
            // 1 saniye gecikme koyuyoruz.
            delay_ms(1000);
            ESPInitCase = 3;
        break;

        case 3:
            // Gelen cevap mode 1 'mi
            strcpy(TempStr,"+CWMODE:1");
            if (strstr(ESP8266Buf,TempStr) != NULL)
            {
                Clear_ESPBuffer();
                fputs("MODE Ayar Dogru",USART2);
                ESPInitCase = 4;
            }
            else
            {
                // Fabrika ayarlarý olarak 2 geliyor biz onu 1 yapip reset komutu ile tamamlariz.
                fputs("AT+CWMODE=1\r\n",USART1);
                fputs("AT+RST\r\n",USART1);
                delay_ms(1000);

                Clear_ESPBuffer();
                fputs("MOD Degistirilir.",USART2);
                ESPInitCase = 0;
            }
        break;

        case 4:
            // Baglanilacak olan wifi agina ait kullanici adi ve sifre girisi yapilir.
            fprintf(USART1,"AT+CWJAP=%s\r\n",Network_User_Pass);
            // 1 saniye gecikme koyuyoruz.
            delay_ms(1000);
            ESPInitCase = 5;
        break;

         case 5:
             // Baglanti saglandiginde OK Cevabi alinir
            strcpy(TempStr,"OK");
            if (strstr(ESP8266Buf,TempStr) != NULL)
            {
                Clear_ESPBuffer();
                fputs("Modeme Baglanti yapildi",USART2);
                ESPInitCase = 6;
            }
            else
            {
                delay_ms(1000);
                fputs("Baglanti Bekleniyor.",USART2);
            }
        break;

        case 6:
            // IP adresi nedir diye soruyoruz ?
            fputs("AT+CIFSR\r\n",USART1);
            // 1 saniye gecikme koyuyoruz.
            delay_ms(1000);
            ESPInitCase = 7;
        break;

        case 7:
            // IP alana kadar error bilgisi gonderir. Onu ayýrýrýz. =)
            strcpy(TempStr,"ERROR");
            if (strstr(ESP8266Buf,TempStr) == NULL)
            {
                fputs("Alinan IP = ",USART2);
                // Gelen bilginin 11.karakterinden itibaren IP adresi yaziyor.
                fputs(&ESP8266Buf[11],USART1);
                Clear_ESPBuffer();
                ESPInitCase=8;
            }
            else
            {
                // ERROR der ise tekrar dene
                delay_ms(1000);
                fputs("Tekrar Dene.",USART2);
                Clear_ESPBuffer();
                ESPInitCase=6;
            }

        break;

        case 8:
            // Baglanilacak siteye ait bilgiler girildi.
            fputs("AT+CIPSTART=\"TCP\",\"www.picproje.org\",80\r\n",USART1);
            // 1 saniye gecikme koyuyoruz.
            delay_ms(1000);
            ESPInitCase = 9;
        break;

        case 9:
            // Baglanti kuruldugunda Linked diye bir cevap aliriz.
            strcpy(TempStr,"Linked");
            if (strstr(ESP8266Buf,TempStr) != NULL)
            {
                Clear_ESPBuffer();
                fputs("Server ile baglanti kuruldu",USART2);
                ESPInitCase = 10;
            }
            else
            {
                // Cevap gelene kadar bekler
                delay_ms(1000);
                fputs("Baglanti Bekleniyor.",USART2);
            }
        break;

        case 10:
            // Artýk HTTP Request bilgimizi gonderebiliriz.
            // CIPSEND ile kac byte bilgi gondereceksek o kadar yazarýz.
            fputs("AT+CIPSEND=100\r\n",USART1);
            // Komutu verdikten sonra bize '>' bilgisi geliyor.
            delay_ms(1000);
            // HTTP Request Hazirlanir.
            fputs("GET / HTTP/1.1\r\n"USART1);
            fputs("Host: www.picproje.org\r\n",USART1);
            fputs("Accept: */*\r\n",USART1);
            fputs("Content-Type: text/html\r\n",USART1);
            fputs("Content-Length: 0\r\n\r\n\r\n",USART1);

            // Buradan sonra http request'e gelen cevaplari degerlendirerek
            // Ne yapmak istiyorsanýz onu yapabilirsiniz =)
            
            while(1);
        break;

    }
}

