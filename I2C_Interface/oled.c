/***************************************************
//Web: http://www.buydisplay.com
EastRising Technology Co.,LTD
Examples for ER-OLEDM0.66-1
Display is Hardward I2C 2-Wire I2C Interface 
Tested and worked with: 
Works with Raspberry pi
****************************************************/

#include <bcm2835.h>
#include <stdio.h>
#include <time.h>
#include "ssd1306.h"

char value[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
int main(int argc, char **argv)
{
    time_t now;
    struct tm *timenow;
    if(!bcm2835_init())
    {
        return -1;
    }
    printf("OLED begin\r\n");
    SSD1306_begin();
    printf("OLED bitmap\r\n");
    SSD1306_bitmap(0, 0, PIC1, 64, 48);
    SSD1306_display();
    bcm2835_delay(3000);
    printf("OLED clear\r\n");
    SSD1306_clear();
    printf("OLED bitmap\r\n");
    SSD1306_bitmap(0, 0, PIC2, 64, 48);
    SSD1306_display();
    bcm2835_delay(3000);
    printf("OLED clear\r\n");
    SSD1306_clear();
    printf("OLED bitmap\r\n");
    SSD1306_bitmap(0, 0, Signal816, 16, 8); 
    SSD1306_bitmap(19, 0, Bluetooth88, 8, 8); 
    SSD1306_bitmap(30, 0, Msg816, 16, 8); 
    SSD1306_bitmap(48, 0, Bat816, 16, 8); 
    SSD1306_string(2, 8, "EastRising", 12, 1); 
    SSD1306_string(2, 20, "buydisplay", 12, 1); 
    SSD1306_string(0, 32, "OLED-DIS", 16, 1); 
    SSD1306_display();
    bcm2835_delay(3000);
    printf("OLED clear\r\n");
    SSD1306_clear();
    printf("OLED display time\r\n");
    while(1)
    {
        time(&now);
        timenow = localtime(&now);

        SSD1306_string(2, 0, "EastRising", 12, 1); 
     
	if((timenow->tm_sec % 2)==0)
        SSD1306_string(2, 36, "buydisplay", 12, 0); 
        else
        SSD1306_string(2, 36, "buydisplay", 12, 1); 


        SSD1306_char(0,  16, value[timenow->tm_hour / 10], 16, 1);
        SSD1306_char(8,  16, value[timenow->tm_hour % 10], 16, 1);
        SSD1306_char(16, 16, ':' , 16, 1);
        SSD1306_char(24, 16, value[timenow->tm_min / 10], 16, 1);
        SSD1306_char(32, 16, value[timenow->tm_min % 10], 16, 1);
        SSD1306_char(40, 16, ':' , 16, 1);
        SSD1306_char(48, 16, value[timenow->tm_sec / 10], 16, 1);
        SSD1306_char(56, 16, value[timenow->tm_sec % 10], 16, 1);

    
        SSD1306_display();


    }
    bcm2835_i2c_end();
    bcm2835_close();
    return 0;
}

