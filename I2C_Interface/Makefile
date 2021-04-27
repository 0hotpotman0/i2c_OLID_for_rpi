oled:oled.o ssd1306.o
	gcc -Wall -o oled oled.o ssd1306.o -lbcm2835
ssd1306.o:ssd1306.c ssd1306.h
	gcc -Wall -c ssd1306.c -lbcm2835
oled.o:oled.c ssd1306.h 
	gcc -Wall -c oled.c -lbcm2835
clean:
	rm ssd1306.o oled.o oled
