// fic = 50 MHz


#include <stdio.h> 
#include <string.h>
#include "lcd.h"
#include "tm4c123gh6pm.h"
#include "uart\UART.h"
#include "pll\pll.h"
#include "uart\UART.h"

void mcu_init(void)
{
	
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;
	delay = SYSCTL_RCGC2_R;
	
	GPIO_PORTB_AMSEL_R = 0x00;
	GPIO_PORTB_PCTL_R = 0x00000000;
	GPIO_PORTB_DIR_R = 0xFF;
	GPIO_PORTB_AFSEL_R = 0x00;
	//GPIO_PORTB_PUR_R = 0x00;
	GPIO_PORTB_DEN_R = 0xFF;
	
	GPIO_PORTE_AMSEL_R = 0x00;
	GPIO_PORTE_PCTL_R = 0x00000000;
	GPIO_PORTE_DIR_R = 0xFF;
	GPIO_PORTE_AFSEL_R = 0x00;
	//GPIO_PORTE_PUR_R = 0x00;
	GPIO_PORTE_DEN_R = 0xFF;
	
	LCD_DATA = 0x00;
	LCD_CONTROL = ~(0x1<<LCD_RS | 0x1<<LCD_RW | 0x1<<LCD_EN) & LCD_CONTROL;
	
	LCD_DATA_DIR = 0xFF;
	LCD_CONTROL_DIR = (0x1<<LCD_RS | 0x1<<LCD_RW | 0x1<<LCD_EN)  | LCD_CONTROL;
	
}

// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = NVIC_ST_RELOAD_M;  // maximum reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it
                                        // enable SysTick with core clock
  NVIC_ST_CTRL_R = NVIC_ST_CTRL_ENABLE+NVIC_ST_CTRL_CLK_SRC;
}
// Time delay using busy wait.
// The delay parameter is in units of the 6 MHz core clock. (167 nsec)
void SysTick_Wait(unsigned long delay){
  volatile unsigned long elapsedTime;
  unsigned long startTime = NVIC_ST_CURRENT_R;
  do{
    elapsedTime = (startTime-NVIC_ST_CURRENT_R)&0x00FFFFFF;
  }
  while(elapsedTime <= delay);
}
// Time delay using busy wait.
// 10000us equals 10ms
void SysTick_Wait1ms(unsigned long delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    SysTick_Wait(50000);  // wait 10ms (assumes 6 MHz clock)
  }
}

int main(void) {
	char ret[17];
	unsigned char hodiny = 12, minuty = 45, sekundy = 22;
	
	PLL_Init();
	UART_Init();
	mcu_init();
	lcd_init4();
	SysTick_Init();
	
	while(1) {
		SysTick_Wait1ms(1000);
		sekundy++;
		
		if(sekundy == 60) {
			sekundy = 0;
			minuty++;
		}
		
		if(minuty == 60) {
			minuty = 0;
			hodiny++;
		}
		
		
		// poslat do putty (COM3)
		UART_OutString(ret);
		UART_OutChar(CR); // od-
		UART_OutChar(LF); // radkovani
		///lcd_print(1, 0, ret);
		//UART_InString(ret, 16);
		
		
		//Formatovani casu
		sprintf(ret, "%02u:%02u:%02u", hodiny, minuty, sekundy);
		lcd_print(0, 0, ret);
	}

}
