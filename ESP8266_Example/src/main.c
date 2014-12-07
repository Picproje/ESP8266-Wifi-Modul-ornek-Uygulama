/*****************************************************************************
*
* File Name        : ESP8266_Example.c
*
* Description      : ESP8266 Modulune ait uygulamalar
*
* Author           : Picproje & MrDarK
*
* Additional Information :
*
*******************************************************************************/

#include "stm32f4xx_conf.h"
#include <stdio.h>
#include <string.h>

#define ESP8266BUFFER_LENGHT 500

char g_arrui8ESP8266Buf[ESP8266BUFFER_LENGHT];
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
        g_arrui8ESP8266Buf[i] = 0;

    ESPWriteIndex=0;
}
/***********************************************************
* Function Name  : Delay
* Description    : Yazılımsal Gecikme Fonksiyonu.
* Input          : Gecikme Degeri
* Return         : None
***********************************************************/
void Delay(__IO uint32_t nCount) {
  while(nCount--) {
  }
}

/***********************************************************
* Function Name  : USART_puts
* Description    : Girdiðimiz cümle ilgili buffer'dan çýkar.
* Input          : USARTx
* Return         : None
***********************************************************/
void USART_puts(USART_TypeDef* USARTx, volatile char *s)
{
	while(*s){
		// wait until data register is empty
		while( !(USARTx->SR & 0x00000040) );
		USART_SendData(USARTx, *s);
		*s++;
	}
}

/***********************************************************
* Function Name  : Usart_InitializeHW
* Description    :
* Input          : None
* Return         : None
***********************************************************/
void Usart_InitializeHW(void)
{
    GPIO_InitTypeDef     GPIO_InitStruct;

    // B Portuna Clock Verilir
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    /**
    * PB6 ve PB7 Pinleri alternatif fonksiyon olarak çalýþacaðý tanýmlanýr
    * @onemli Usartý aktif etmeden önce çaðrýlmalýdýr.!
    */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1); //	TX()
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1); //	RX()
    // Initialize pins as alternating function
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3); //	TX()
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3); //	RX()
    // Initialize pins as alternating function
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_ResetBits(GPIOB,GPIO_Pin_13);
}

/***********************************************************
* Function Name  : Usart_InitializeAp
* Description    :
* Input          : None
* Return         : None
***********************************************************/
void Usart_InitializeAp(void)
{
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStructure;
    /**
     * USART1 modülüne Clock verilir.
     */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    /**
     * USART3 modülüne Clock verilir.
     */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    /**
     * Baudrate 115200
     * Hardware Flow Kapali
     * Usart Full-Duplex çalisacak TX ve RX modu aktif edilir.
     * Parity Kullanmiyoruz.
     * 1 stop byte mevcut
     * Data bit uzunlugumuz 8
     *
     * USART1 Kurulup
     * Aktif edilir.
     */
    USART_InitStruct.USART_BaudRate = 115200;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &USART_InitStruct);

    USART_InitStruct.USART_BaudRate = 115200;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART3, &USART_InitStruct);
    USART_Cmd(USART3, ENABLE);  // USART3 aktif edilir.

	/* USART1 Rx interrupt aktif ediliyor
	 * USART'dan herhangi bir data geldiginde
	 * USART1_IRQHandler() adiyla isimlendirdigimiz fonksiyona gider
	 */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // USART1 Rx interrupt aktif ediliyor

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		 // USART1 interrupt ina ince ayar vermek istiyoruz.
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;// Öncelik olarak bu interrupt kaynagina 0 veriyoruz. ( 0 yazarak En öncelikli kaynak yapiyoruz )
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		 // Kendi bagli olduðu vektordeki alt grup içinde de en öncelikli olarak kurduk
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			 // USART1 interrupt kanali aktif edilir.
	NVIC_Init(&NVIC_InitStructure);							 // Yaptigimiz ayarlari NVIC birimine yüklüyoruz.

    USART_Cmd(USART1, ENABLE);  // Nihayetinde USART1 aktif edilir.
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

    // Modul Reset Pini High yapiliyor.
    GPIO_SetBits(GPIOB,GPIO_Pin_13);

    switch(ESPInitCase)
    {
        case 0:
            // Standart AT Komutu gonderilir , Cevabi OK olmalidir.
            USART_puts(USART1,"AT\r\n");
            // 1 saniye gecikme koyuyoruz.
            Delay(1000000L);
            ESPInitCase = 1;
        break;

        case 1:
            // STRSTR fonksiyonu metin icinde metin arayan bir fonksiyondur
            // Eger aranilan metni bulabilirse pointer adresi ile geri donus yapar
            // Bulamaz ise NULL dondurur. Bizde null'dan farklı mı diye sorariz.
            if (strstr(g_arrui8ESP8266Buf,"OK") != NULL)
            {
                Clear_ESPBuffer();
                USART_puts(USART3,"Module Erisildi");
                ESPInitCase = 2;
            }
            else
            {
                Clear_ESPBuffer();
                USART_puts(USART3,"Modul Bulunamadi, Tekrar Deneniyor");
                ESPInitCase = 0;
            }
        break;

        case 2:
            // Modulun 3 modu var gerekli bilgi datasheet'de biz 1 olmasini istiyoruz
            USART_puts(USART1,"AT+CWMODE?\r\n");
            // 1 saniye gecikme koyuyoruz.
            Delay(1000000L);
            ESPInitCase = 3;
        break;

        case 3:
            // Gelen cevap mode 1 'mi
            if (strstr(g_arrui8ESP8266Buf,"+CWMODE:1") != NULL)
            {
                Clear_ESPBuffer();
                USART_puts(USART3,"MODE Ayar Dogru");
                ESPInitCase = 4;
            }
            else
            {
                // Fabrika ayarları olarak 2 geliyor biz onu 1 yapip reset komutu ile tamamlariz.
                USART_puts(USART1,"AT+CWMODE=1\r\n");
                USART_puts(USART1,"AT+RST\r\n");
                Delay(1000000L);

                Clear_ESPBuffer();
                USART_puts(USART3,"MOD Degistirilir.");
                ESPInitCase = 0;
            }
        break;

        case 4:
            // Baglanilacak olan wifi agina ait kullanici adi ve sifre girisi yapilir.
            USART_puts(USART1,"AT+CWJAP=\"MrDarK\",\"oztekin006\"\r\n");
            // 1 saniye gecikme koyuyoruz.
            Delay(2000000L);
            ESPInitCase = 5;
        break;

         case 5:
             // Baglanti saglandiginde OK Cevabi alinir
            if (strstr(g_arrui8ESP8266Buf,"OK") != NULL)
            {
                Clear_ESPBuffer();
                USART_puts(USART3,"Modeme Baglanti yapildi");
                ESPInitCase = 6;
            }
            else
            {
                Delay(1000000L);
                USART_puts(USART3,"Baglanti Bekleniyor.");
            }
        break;

        case 6:
            // IP adresi nedir diye soruyoruz ?
            USART_puts(USART1,"AT+CIFSR\r\n");
            // 1 saniye gecikme koyuyoruz.
            Delay(1000000L);
            ESPInitCase = 7;
        break;

        case 7:
            // IP alana kadar error bilgisi gonderir. Onu ayırırız. =)
            if (strstr(g_arrui8ESP8266Buf,"ERROR") == NULL)
            {
                USART_puts(USART3,"Alinan IP = ");
                // Gelen bilginin 11.karakterinden itibaren IP adresi yaziyor.
                USART_puts(USART3,&g_arrui8ESP8266Buf[11]);
                Clear_ESPBuffer();
                ESPInitCase=8;
            }
            else
            {
                // ERROR der ise tekrar dene
                Delay(1000000L);
                USART_puts(USART3,"Tekrar Dene.");
                Clear_ESPBuffer();
                ESPInitCase=6;
            }

        break;

        case 8:
            // Baglanilacak siteye ait bilgiler girildi.
            USART_puts(USART1,"AT+CIPSTART=\"TCP\",\"www.picproje.org\",80\r\n");
            // 1 saniye gecikme koyuyoruz.
            Delay(1000000L);
            ESPInitCase = 9;
        break;

        case 9:
            // Baglanti kuruldugunda Linked diye bir cevap aliriz.
            if (strstr(g_arrui8ESP8266Buf,"Linked") != NULL)
            {
                Clear_ESPBuffer();
                USART_puts(USART3,"Server ile baglanti kuruldu");
                ESPInitCase = 10;
            }
            else
            {
                // Cevap gelene kadar bekler
                Delay(1000000L);
                USART_puts(USART3,"Baglanti Bekleniyor.");
            }
        break;

        case 10:
            // Artık HTTP Request bilgimizi gonderebiliriz.
            // CIPSEND ile kac byte bilgi gondereceksek o kadar yazarız.
            USART_puts(USART1,"AT+CIPSEND=100\r\n");
            // Komutu verdikten sonra bize '>' bilgisi geliyor.
            Delay(1000000L);
            // HTTP Request Hazirlanir.
            USART_puts(USART1,"GET / HTTP/1.1\r\n");
            USART_puts(USART1,"Host: www.picproje.org\r\n");
            USART_puts(USART1,"Accept: */*\r\n");
            USART_puts(USART1,"Content-Type: text/html\r\n");
            USART_puts(USART1,"Content-Length: 0\r\n\r\n\r\n");

            // Buradan sonra http request'e gelen cevaplari degerlendirerek
            // Ne yapmak istiyorsanız onu yapabilirsiniz =)
            // Herkese bol sanslar ! Iyi gunler efenim.
            while(1);
        break;

    }
}

int main(void)
{
    // System Clock Init Edilir.
    SystemInit();
    // Usart modullerinin portlarının hardware ayarlari yapilir.
    Usart_InitializeHW();
    // Usart modullerinin clock , baud ve interrupt ayarlari yapilir.
    Usart_InitializeAp();
    // Giris metnimiz =)
    USART_puts(USART3,"MrDarK & PicProje ESP8266 Example Starting..! \r\n");
    // Komut gondermeden once module ait bufferı temizleriz.
    Clear_ESPBuffer();

    while(1)
    {
        // Bakmayın init yazdığına her biseyi yapiyor =)
        ESP8266_Init();
    }


}

// USART1 interrupt Alt Programi
void USART1_IRQHandler(void){
    // USART1 RX interrupt flag kontrol edilir.
    if( USART_GetITStatus(USART1, USART_IT_RXNE) )
    {
        uint8_t Received_Byte = USART1->DR; // Gelen bilgi değişken içine alinir.
        USART3->DR = Received_Byte;         // Debug monitor icin ayni bilgi usart3'e gonderilir.

        // Strstr fonksiyonu için eklendi, modülden null karakteri gelebiliyordu , onu engellemis olduk.
        if(Received_Byte != 0)
        {
            g_arrui8ESP8266Buf[ESPWriteIndex] = Received_Byte;
            ESPWriteIndex++;
        }

    }
}









