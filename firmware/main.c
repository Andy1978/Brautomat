/*************************************************

	Brautomat

	Autor: Andreas Weber 
	src: https://github.com/Andy1978/Brautomat
	changlog: 02.03.2013 angelegt

	Infos:	
	H-Brücken PWM Treiber IR2104
	IN obere Halbbrücke PD4, OC1B
	/SD obere Halbbrücke PD6
	IN untere Halbbrücke PD5, OC1A
	/SD untere Halbbrücke PD7

**************************************************/

#define F_CPU 16000000UL

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "lcd.h"

ISR(TIMER0_COMP_vect) //1kHz
{
    static uint8_t disp_cnt=0;
    
    if (!disp_cnt++)
    {
    	lcd_gotoxy(0,1);
    	char buf[20];
    	ltoa(ADC,buf,10);
    	lcd_puts(buf);
    	lcd_puts_P("    ");
    }
}

int8_t sin_list[]={0,24,48,70,89,105,117,124,127,124,117,105,89,70,48,24,0,-24,-48,-70,-89,-105,-117,-124,-127,-124,-117,-105,-89,-70,-48,-24};

ISR(ADC_vect) 
{
	//uint16_t temp=ADC;
	//OCR1A=temp;
	//OCR1B=1023-temp;
	
	int16_t temp=ADC-512;
	OCR1A=512-temp;
	OCR1B=512+temp;
	
/*
	static float i=0;
	i+=ADC/50000.0;
	if (i>32) i-=32;
	OCR1A=512-sin_list[(uint8_t)i];
	OCR1B=512+sin_list[(uint8_t)i];
*/	
}
int main(void)
{
    lcd_init(LCD_DISP_ON);
    lcd_clrscr();
    lcd_puts_P("Brautomat 0.1\n");
    
    DDRD |= _BV(PD4) | _BV(PD5) | _BV(PD6) | _BV(PD7);
    PORTD |= _BV(PD6) | _BV(PD7);
    
	/*** TIMER0 ***/
	OCR0=250;
	//CTC = Clear Timer on Compare match S.80
	//Normal port operation, OC0 disconnected
	//Prescaler=64 -> clk=250kHz
	TCCR0 = _BV(WGM01) | _BV(CS01) | _BV(CS00); 
	//On Compare match Interrupt Enable for timer 0
	TIMSK |= _BV(OCIE0);

	/** TIMER1 **/
	//PWM Phase correct 10bit
	//Set OC1A+OC1B on match when upcounting (page 108)
	TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(COM1A0) | _BV(COM1B0) | _BV(WGM11) | _BV(WGM10);
	//Prescaler = 1 (page 110)
	TCCR1B = _BV(CS10);
	

	/*** ADC ***/
	//Prescaler 64 = 250Khz ADC Clock, AutoTrigger, Interrupts enable
	ADCSRA = _BV(ADEN) | _BV(ADPS1) | _BV(ADPS2) | _BV(ADATE) | _BV(ADSC) | _BV(ADIE); 
	//Interne 2.56V Reference verwenden
	//Multiplexer auf Kanal2 (Poti)
	ADMUX = (_BV(REFS0) | _BV(REFS1)) + 2; 	
	//ADC in Free Running mode
	SFIOR &= ~(_BV(ADTS2) | _BV(ADTS1) | _BV(ADTS0));	

	//enable global interrupts
    sei();			
	
    for (;;)    /* main event loop */
    {
      //do nothing
    }
    return 0;
}
