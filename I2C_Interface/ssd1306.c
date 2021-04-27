#include <bcm2835.h>
#include <stdio.h>
#include "ssd1306.h"
#define VCCSTATE SSD1306_SWITCHCAPVCC
#define WIDTH 64
#define HEIGHT 48
#define PAGES 6

#define RST 25
//I2C
#define IIC_CMD        0X00
#define IIC_RAM        0X40
#define command(Reg)  I2C_Write_Byte(Reg, IIC_CMD)
#define data(Data)    I2C_Write_Byte(Data, IIC_RAM)

char buffer[384];



void I2C_Write_Byte(uint8_t value, uint8_t Cmd)
{
    char buf[2] = {Cmd,value};

    int ref = bcm2835_i2c_write(buf, 2);
    while(ref != 0) {
        ref = bcm2835_i2c_write(buf, 2);
        if(ref == 0)
            break;
    }	
}


void SSD1306_begin()
{
    if(!bcm2835_init()) {
        printf("bcm2835 init failed   !!! \r\n");
        return ;
    } else {
        printf("bcm2835 init success  !!! \r\n");
    }

     printf("USE_IIC\r\n");
    bcm2835_i2c_begin();
    bcm2835_i2c_setSlaveAddress(0x3c);  //i2c address
	
	/*
	BCM2835_I2C_CLOCK_DIVIDER_2500 ：2500 = 10us = 100 kHz 
	BCM2835_I2C_CLOCK_DIVIDER_626  ：622 = 2.504us = 399.3610 kHz
	150 = 60ns = 1.666 MHz (default at reset)
	148 = 59ns = 1.689 MHz
	*/	
	bcm2835_i2c_setClockDivider(BCM2835_I2C_CLOCK_DIVIDER_626);
    //  bcm2835_i2c_set_baudrate(100000);    //100k baudrate

    bcm2835_gpio_fsel(RST, BCM2835_GPIO_FSEL_OUTP);

    bcm2835_gpio_write(RST, HIGH);
    bcm2835_delay(10);
    bcm2835_gpio_write(RST, LOW);
    bcm2835_delay(10);
    bcm2835_gpio_write(RST, HIGH);

    command(0xae);//--turn off oled panel
	
    command(0xd5);//--set display clock divide ratio/oscillator frequency
    command(0x80);//--set divide ratio

    command(0xa8);//--set multiplex ratio
    command(0x2f);//--1/48 duty

    command(0xd3);//-set display offset
    command(0x00);//-not offset

    command(0x8d);//--set Charge Pump enable/disable
    command(0x14);//--set(0x10) disable

    command(0x40);//--set start line address

    command(0xa6);//--set normal display

    command(0xa4);//Disable Entire Display On

    command(0xa1);//--set segment re-map 128 to 0

    command(0xC8);//--Set COM Output Scan Direction 64 to 0

    command(0xda);//--set com pins hardware configuration
    command(0x12);

    command(0x81);//--set contrast control register
    command(0xcf);

    command(0xd9);//--set pre-charge period
    command(0xf1);

    command(0xdb);//--set vcomh
    command(0x40);

    command(0xaf);//--turn on oled panel

}
void SSD1306_clear()
{
    int i;
    for(i = 0; i < sizeof(buffer); i++)
    {
        buffer[i] = 0;
    }
}
void SSD1306_pixel(int x, int y, char color)
{
    if(x > WIDTH || y > HEIGHT)return ;
    if(color)
        buffer[x+(y/8)*WIDTH] |= 1<<(y%8);
    else
        buffer[x+(y/8)*WIDTH] &= ~(1<<(y%8));
}
void SSD1306_char1616(uint8_t x, uint8_t y, uint8_t chChar)
{
    uint8_t i, j;
    uint8_t chTemp = 0, y0 = y, chMode = 0;

    for (i = 0; i < 32; i ++) {
        chTemp = Font1612[chChar - 0x30][i];
        for (j = 0; j < 8; j ++) {
            chMode = chTemp & 0x80? 1 : 0; 
            SSD1306_pixel(x, y, chMode);
            chTemp <<= 1;
            y ++;
            if ((y - y0) == 16) {
                y = y0;
                x ++;
                break;
            }
        }
    }
}

void SSD1306_char3216(uint8_t x, uint8_t y, uint8_t chChar)
{
    uint8_t i, j;
    uint8_t chTemp = 0, y0 = y, chMode = 0;

    for (i = 0; i < 64; i ++) {
        chTemp = Font3216[chChar - 0x30][i];
        for (j = 0; j < 8; j ++) {
            chMode = chTemp & 0x80? 1 : 0; 
            SSD1306_pixel(x, y, chMode);
            chTemp <<= 1;
            y ++;
            if ((y - y0) == 32) {
                y = y0;
                x ++;
                break;
            }
        }
    }
}
void SSD1306_char(unsigned char x, unsigned char y, char acsii, char size, char mode)
{
    unsigned char i, j, y0=y;
    char temp;
    unsigned char ch = acsii - ' ';
    for(i = 0;i<size;i++) {
        if(size == 12)
        {
            if(mode)temp=Font1206[ch][i];
            else temp = ~Font1206[ch][i];
        }
        else 
        {            
            if(mode)temp=Font1608[ch][i];
            else temp = ~Font1608[ch][i];
        }
        for(j =0;j<8;j++)
        {
            if(temp & 0x80) SSD1306_pixel(x, y, 1);
            else SSD1306_pixel(x, y, 0);
            temp <<=1;
            y++;
            if((y-y0)==size)
            {
                y = y0;
                x ++;
                break;
            }
        }
    }
}
void SSD1306_string(uint8_t x, uint8_t y, const char *pString, uint8_t Size, uint8_t Mode)
{
    while (*pString != '\0') {       
        if (x > (WIDTH - Size / 2)) {
            x = 0;
            y += Size;
            if (y > (HEIGHT - Size)) {
                y = x = 0;
            }
        }
        
        SSD1306_char(x, y, *pString, Size, Mode);
        x += Size / 2;
        pString ++;
    }
}

void SSD1306_bitmap(unsigned char x, unsigned char y, const unsigned char *pBmp, unsigned char chWidth, unsigned char chHeight)
{
    unsigned char i, j, byteWidth = (chWidth + 7)/8;
    for(j = 0; j < chHeight; j++) {
        for(i = 0;i <chWidth;i ++) {
            if(*(pBmp + j*byteWidth + i/8) & (128 >> (i & 7))) {
                SSD1306_pixel(x+i, y+j, 1);
            }
        }
    }        
}

void SSD1306_display()
{ 
 int i;
    uint8_t page;
    char *pBuf = (char *)buffer;
    
    for (page = 0; page < 6; page++) {  
        // set page address 
        command(0xB0 + page);
        // set low column address
        command(0x00); 
        // set high column address 
        command(0x12); 
        // write data 
        for(i = 0; i< WIDTH; i++ ) {
            data(pBuf[i+page*WIDTH]);
        }
    }

}
