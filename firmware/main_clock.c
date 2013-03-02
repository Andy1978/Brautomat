/*************************************************

	Beispiel 7

	H-Brücken PWM Treiber IR2104
	IN obere Halbbrücke PD4, OC1B
	/SD obere Halbbrücke PD6
	IN untere Halbbrücke PD5, OC1A
	/SD untere Halbbrücke PD7
		
	Autor: Andreas Weber 20.12.2007

**************************************************/

#define F_CPU 16000000UL

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "lcd.h"

char temp[]=__TIME__;
uint8_t seconds, minutes, hours;

ISR(TIMER0_COMP_vect) //1kHz
{
	static uint16_t count=0;
	static uint8_t half_sec=0;
	count++;
	
	if (count==1000)
	{
		PORTA ^= _BV(PA4) | _BV(PA5);
		count=0;
		seconds++;
		if (seconds==60)
		{
			minutes++;
			seconds=0;
		}	
		if (minutes==60)
		{
			hours++;
			minutes++;
		}	
		if (hours==24)
		{
			hours=0;	
		}			
		
		char buffer[5];
		lcd_gotoxy(2,1);
		itoa(hours,buffer,10);
		lcd_puts(buffer);
		lcd_puts_P("h ");
		itoa(minutes,buffer,10);
		lcd_puts(buffer);
		lcd_puts_P("m ");
		itoa(seconds,buffer,10);
		lcd_puts(buffer);
		lcd_puts_P("s ");
	}
}

int main(void)
{
    lcd_init(LCD_DISP_ON);
    lcd_clrscr();
    lcd_puts_P("Bsp 7 IR2104 PWM\n");
    char* p;
    hours=strtol(temp,&p,10);
    p++;
    minutes=strtol(p,&p,10);
    p++;
    seconds=strtol(p,&p,10);
    
    DDRA |= _BV(PA4) | _BV(PA5);

	/*** TIMER0 ***/
	OCR0=250;
	//CTC = Clear Timer on Compare match S.80
	//Normal port operation, OC0 disconnected
	//Prescaler=64 -> clk=250kHz
	TCCR0 = _BV(WGM01) | _BV(CS01) | _BV(CS00); 
	//On Compare match Interrupt Enable for timer 0
	TIMSK |= _BV(OCIE0);


	/*** ADC ***/
	//Prescaler 32 = 500Khz ADC Clock, AutoTrigger
	ADCSRA = _BV(ADEN) | _BV(ADPS0) | _BV(ADPS2) | _BV(ADATE) | _BV(ADSC); 
	//Interne 2.56V Reference verwenden
	//Multiplexer auf Kanal2 (Poti)
	//nur 8bit, left adjusted
	ADMUX = (_BV(REFS0) | _BV(REFS1) | _BV(ADLAR)) + 1; 	
	//ADC in Free Running mode
	SFIOR &= ~(_BV(ADTS2) | _BV(ADTS1) | _BV(ADTS0));	

	//enable global interrupts
    sei();			
	
    for (;;)    /* main event loop */
    {
    }
    return 0;
}
