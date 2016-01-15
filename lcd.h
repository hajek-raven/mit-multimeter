

#include "tm4c123gh6pm.h"
#include <string.h>

//
#define LCD_DATA GPIO_PORTB_DATA_R
#define LCD_DATA_DIR GPIO_PORTB_DIR_R

#define LCD_CONTROL GPIO_PORTE_DATA_R
#define LCD_CONTROL_DIR GPIO_PORTE_DIR_R

#define LCD_RS 0
#define LCD_RW 1
#define LCD_EN 2

#define LCD_FUNCTION_SET 0x38 //0b000DNF--
#define LCD_DISPLAY_ON_OFF_CONTROL 0x0C //0b000001DCB
#define LCD_DISPLAY_CLEAR 0x01 //0b00000001
#define LCD_ENTRY_MODE_SET 0x06 //0b00000110
#define LCD_RETURN_HOME 0x02 //0b00000010
#define LCD_2ND_ROW 0x40
#define LCD_SET_DDRAM_ADDRESS 0x80 //0b10000000
#define LCD_SET_CGRAM_ADDRESS 0x40 //0b00101000

#define LCD_FUNCTION_SET8 0x38 // 8bit
#define LCD_FUNCTION_SET4 0x28 // 4 bit

//globální promenné
unsigned int i;
//hlavicky (signatury) podprogramu
void mcu_init(void);
void lcd_init(void);
void delay_ms(volatile unsigned int cnt);
void delay_250ns(volatile unsigned int cnt);
void delay_us(volatile unsigned int cnt);
void lcd_cmd8(volatile unsigned char cmd);
void lcd_data8(unsigned char cmd);
void lcd_cmd4(volatile unsigned char cmd);
void lcd_data4(unsigned char cmd);
void lcd_print(unsigned char radek, unsigned char sloupec, char * retez);
void lcd_def_char(char ascii_code, unsigned char * retez);




void lcd_init8(void)
{
	//LCD_PWR = ON;
	delay_ms(30);
	lcd_cmd8(LCD_FUNCTION_SET);
	lcd_cmd8(LCD_DISPLAY_ON_OFF_CONTROL);
	lcd_cmd8(LCD_DISPLAY_CLEAR);
	lcd_cmd8(LCD_ENTRY_MODE_SET);
}

void lcd_init4(void)
{
	//LCD_PWR = ON;
	delay_ms(30);
	lcd_cmd8(LCD_FUNCTION_SET4);
	lcd_cmd4(LCD_FUNCTION_SET4);
	lcd_cmd4(LCD_DISPLAY_ON_OFF_CONTROL);
	lcd_cmd4(LCD_DISPLAY_CLEAR);
	lcd_cmd4(LCD_ENTRY_MODE_SET);
}

void lcd_cmd8(unsigned char cmd)
{
	LCD_CONTROL = ~(0x1<<LCD_RS | 0x1<<LCD_RW) & LCD_CONTROL;//vynulovat LCD_RS a LCD_RW - 60ns
	delay_250ns(1);
	LCD_CONTROL = (0x1<<LCD_EN)  | LCD_CONTROL;//LCD_EN = 1
	LCD_DATA = cmd;//LCD_DATA <- cmd
	delay_250ns(1);
	LCD_CONTROL = ~(0x1<<LCD_EN) & LCD_CONTROL;//LCD_EN = 0
	delay_250ns(1);
	
	if( cmd == LCD_DISPLAY_CLEAR || cmd == LCD_RETURN_HOME)
	delay_us(1550);
	
	else delay_us(50);
	
}

void lcd_cmd4(unsigned char cmd)
{
	LCD_CONTROL = ~(0x1<<LCD_RS | 0x1<<LCD_RW) & LCD_CONTROL;//vynulovat LCD_RS a LCD_RW - 60ns
	delay_250ns(1);
	LCD_CONTROL = (0x1<<LCD_EN)  | LCD_CONTROL;//LCD_EN = 1
	LCD_DATA = cmd;//LCD_DATA <- cmd
	delay_250ns(1);
	LCD_CONTROL = ~(0x1<<LCD_EN) & LCD_CONTROL;//LCD_EN = 0
	delay_250ns(1);
	
	LCD_CONTROL = ~(0x1<<LCD_RS | 0x1<<LCD_RW) & LCD_CONTROL;//vynulovat LCD_RS a LCD_RW - 60ns
	delay_250ns(1);
	LCD_CONTROL = (0x1<<LCD_EN)  | LCD_CONTROL;//LCD_EN = 1
	LCD_DATA = cmd<<4;//LCD_DATA <- cmd
	delay_250ns(1);
	LCD_CONTROL = ~(0x1<<LCD_EN) & LCD_CONTROL;//LCD_EN = 0
	delay_250ns(1);
	
	if( cmd == LCD_DISPLAY_CLEAR || cmd == LCD_RETURN_HOME)
	delay_us(1550);
	
	else delay_us(50);
	
}

void lcd_data8(unsigned char cmd)
{
	LCD_CONTROL = ~(0x1<<LCD_RW) & LCD_CONTROL;//vynulovat LCD_RW
	LCD_CONTROL = (0x1<<LCD_RS)  | LCD_CONTROL;//nastavit LCD_RS na 1
	delay_250ns(1);
	LCD_CONTROL = (0x1<<LCD_EN)  | LCD_CONTROL;//LCD_EN = 1
	LCD_DATA = cmd;//LCD_DATA <- cmd
	delay_250ns(1);
	LCD_CONTROL = ~(0x1<<LCD_EN) & LCD_CONTROL;//LCD_EN = 0
	delay_250ns(1);
	delay_us(50);
	
}

void lcd_data4(unsigned char cmd)
{
	LCD_CONTROL = ~(0x1<<LCD_RW) & LCD_CONTROL;//vynulovat LCD_RW
	LCD_CONTROL = (0x1<<LCD_RS)  | LCD_CONTROL;//nastavit LCD_RS na 1
	delay_250ns(1);
	LCD_CONTROL = (0x1<<LCD_EN)  | LCD_CONTROL;//LCD_EN = 1
	LCD_DATA = cmd;//LCD_DATA <- cmd
	delay_250ns(1);
	LCD_CONTROL = ~(0x1<<LCD_EN) & LCD_CONTROL;//LCD_EN = 0
	delay_250ns(1);
	
	LCD_CONTROL = ~(0x1<<LCD_RW) & LCD_CONTROL;//vynulovat LCD_RW
	LCD_CONTROL = (0x1<<LCD_RS)  | LCD_CONTROL;//nastavit LCD_RS na 1
	delay_250ns(1);
	LCD_CONTROL = (0x1<<LCD_EN)  | LCD_CONTROL;//LCD_EN = 1
	LCD_DATA = cmd<<4;//LCD_DATA <- cmd
	delay_250ns(1);
	LCD_CONTROL = ~(0x1<<LCD_EN) & LCD_CONTROL;//LCD_EN = 0
	delay_250ns(1);
	
	
	delay_us(50);
	
}

void lcd_print(unsigned char radek, unsigned char sloupec, char * retez)
{
	
	if(radek & 0x01)
		sloupec = sloupec + LCD_2ND_ROW;
	
	if(sloupec > (39 - strlen(retez))) 
		sloupec = 39 - strlen(retez);
	
	lcd_cmd4(LCD_SET_DDRAM_ADDRESS | sloupec);
	{
		
			for(i=0;i<strlen(retez);i++)
			{
				lcd_data4(retez[i]);
				
			}
		
			//while (*retez) lcd_data4(*retez++);
	}
	
}

void lcd_def_char(char ascii_code, unsigned char * retez)
{
	ascii_code = ascii_code & 0x07;
	ascii_code = ascii_code << 3; //ascii code bit posuv o 3 bity vlevo //*8	
	lcd_cmd4(LCD_SET_CGRAM_ADDRESS | ascii_code);
	
	for(i=0;i<7;i++)
	{
		lcd_data4(retez[i]);
	}
	lcd_cmd4(LCD_SET_DDRAM_ADDRESS);
}

void delay_ms(volatile unsigned int cnt)
{
	cnt = cnt * 4000;
	while(cnt)
	{
		cnt--;
	}
}

void delay_250ns(volatile unsigned int cnt)
{
	while(cnt)
	{
		cnt--;
	}
}

void delay_us(volatile unsigned int cnt)
{
	cnt = cnt * 4;
	while(cnt)
	{
		cnt--;
	}
}
