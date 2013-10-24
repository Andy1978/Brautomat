/*
    Copyright 2013 Andreas Weber (andy.weber.aw at gmail dot com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*************************************************

  Brautomat AVR firmware

  Autor: Andreas Weber 
  src: https://github.com/Andy1978/Brautomat
  changelog: 02.03.2013 angelegt
             06.04.2013 aw: PT100 nun über Wheatstone Brücke einlesen und DS18B20 
             22.10.2013 aw: PT100 entfernt, nur noch DS18B20

  Infos:  
  H-Brücken PWM Treiber IR2104
  IN obere Halbbrücke PD4, OC1B
  /SD obere Halbbrücke PD6
  IN untere Halbbrücke PD5, OC1A
  /SD untere Halbbrücke PD7

  Scheibenwischermotor Opel Astra Bj 86..89?
  Stecker-Pin Adernfarbe  Funktion
  E           weiß        Motor schnell gegen Masse/Gehäuse
  D           violett/sw  0-Stellung
  C           grün        0-Stellung kurz gegen MAsse
  B
  A           orange      Motor langsam gegen Masse/Gehäuse
   
  Relais für Heizung: PB3

  Maxim DS18B20 an PA6, externe Versorgung 4,7k Pull-Up

  LCD:
  S99.5°C  I77.3°C
  RH....
   
  Buchstaben in der unteren Zeile sind die Bits in s_status.enable
  Bit 0, R: Temperaturregelung aktiv
  Bit 1, H: Heizung eingeschaltet
  noch weiter definieren: Steuerung usw.

**************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "lcd.h"
#include "uart.h"
#include "onewire.h"
#include "ds18x20.h"

//Telegramm AVR zum PC
struct s_status
{
  float temperature;            //Isttemperatur von DS18B20[°C]
  uint8_t aktive_step;          //aktueller Schritt im Ablauf
  uint16_t remaining_step_time; //verbleibende Zeit im aktuellen Schritt [s]
  uint8_t  bits;
  //Bit 0 H: Heizung aktiv
  uint8_t uart_error_cnt;
  uint8_t last_uart_error;
};

//Telegramm vom PC zum AVR
struct s_setvalues
{
  float temperature_set_point;  //Solltemperatur [°C]
  uint8_t amplitude_set_point;  //Amplitude Rührwerk 0-255
  uint8_t period_set_point;     //Periodendauer Rührwerk in 100ms (0=keine Modulation)
  float step_temp[MAX_STEPS];   //Temperatur in der Schrittkette [°C]
  float dT_dt[MAX_STEPS];       //Temperaturanstieg [°C/min]
  uint16_t step_time[MAX_STEPS];//Dauer Schritt [s]
  uint8_t	 bits;
  //Bit 0 A: Temperaturregelung aktiv (Handbetrieb wenn nicht)
  //Bit 1 M: Heizung aktiv im Handbetrieb
  //Bit 2 S: Temperatursollwerte aus Schrittkette
  //Bit 3: Schritt weiter (Flanke)
  //Bit 4: Schritt zurück (Flanke)
};

#define UART_BAUD_RATE 38400
//#define UART_BAUD_RATE 115200
#define MAXSENSORS 5
#define NEWLINESTR "\r\n"
#define OW_ONE_BUS

volatile struct s_status status;
volatile struct s_setvalues setvalues;
volatile uint8_t do_ds18b20_meas;
volatile uint8_t do_update_lcd;

uint8_t uart_error;
uint8_t gSensorID[OW_ROMCODE_SIZE]={ 0x5A, 0xF2, 0xBD, 0x04};


//  Integer (Basis 10) rechtsbündig auf LCD ausgeben.
void lcd_put_int(int16_t val, uint8_t len)
{
  char buffer[len+1];
  itoa(val,buffer,10);
  size_t empty=len-strlen(buffer);
  while(empty--)
    lcd_putc(' ');
  lcd_puts(buffer);
}

void lcd_put_int32(int32_t val, uint8_t len)
{
  char buffer[len+1];
  ltoa(val,buffer,10);
  size_t empty=len-strlen(buffer);
  while(empty--)
    lcd_putc(' ');
  lcd_puts(buffer);
}

void update_lcd(void)
{

  char buf[20];

  //Solltemperatur
  lcd_gotoxy(0,0);
  _delay_ms(2);   //sonst zickt gotoxy rum, TODO: nachprüfen, ggf. Zeit verkleinern
  lcd_puts_P("S");
  dtostrf(setvalues.temperature_set_point,4,1,buf);
  lcd_puts(buf);
  lcd_putc(0xDF);
  lcd_putc('C');

  //Isttemperatur
  //lcd_gotoxy(0,1);
  lcd_puts_P("  I");
  dtostrf(status.temperature,4,1,buf);
  lcd_puts(buf);
  lcd_putc(0xDF);
  lcd_putc('C');

  //Status
  lcd_putc((setvalues.bits & _BV(0))? 'A':' ');
  lcd_putc((setvalues.bits & _BV(1))? 'M':' ');
  lcd_putc((setvalues.bits & _BV(2))? 'S':' ');
  lcd_putc((status.bits & _BV(0))? 'H':' ');

  //aktiver Schritt
  lcd_puts_P(" S:");
  itoa(status.aktive_step,buf,10);
  lcd_puts(buf);
  
  //verbleibende Zeit im Schritt in Sekunden
  sprintf(buf," Z:%5i",status.remaining_step_time);
  lcd_puts(buf);

/*
  lcd_gotoxy(0,0);
  lcd_put_int(status.uart_error_cnt,3);
  lcd_putc(' ');
  lcd_put_int(uart_error,3);
*/
  
}

//ISR(TIMER0_COMP_vect, ISR_NOBLOCK) //1kHz
ISR(TIMER0_COMP_vect) //1kHz
{
  static int16_t cnt=0;
  if (cnt++ == 1000)
  {
    do_ds18b20_meas=1;
    cnt = 0;
  }else if(cnt==500)
    do_update_lcd=1;
}

int8_t sin_list[]={0,24,48,70,89,105,117,124,127,124,117,105,89,70,48,24,0,-24,-48,-70,-89,-105,-117,-124,-127,-124,-117,-105,-89,-70,-48,-24};

ISR(ADC_vect) //ca. 125kHz
{
  int16_t temp=ADC-512;
  OCR1A=512-temp;
  OCR1B=512+temp;
  //OCR1A=1023;
  //OCR1B=0;

  //kurzer Heizungsrelais Test
  if(ADC>500)
    PORTB |= _BV(PB3);
  else
    PORTB &= ~(_BV(PB3));

}

// UART bearbeiten. Es gibt nur ein Telegramm mit allen Sollwerten
// und eine Antwort mit allen Istwerten bzw. Status
void processUART(void)
{
  //Alle Daten empfangen
  while(uart_GetRXCount()>=sizeof(struct s_setvalues))
  {
    //Empfangen
    char* rec=(char*)&setvalues;
    uint8_t i;
    uint16_t rx_tmp;
    for(i=0;i<sizeof(struct s_setvalues);i++)
    {
      rx_tmp=uart_getc();
      rec[i]=rx_tmp & 0xFF;
      if(rx_tmp & 0xFF00)
      {
        status.uart_error_cnt++;
        uart_error=((rx_tmp & 0xFF00) >> 8);
      }  
    }
    //status.bits |= setvalues.bits;
    //Senden
    char* send=(char*)&status;
    for(i=0;i<sizeof(struct s_status);i++)
    {
      uart_putc(send[i]); 
    }
  }
}

int main(void)
{
  //uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(UART_BAUD_RATE,F_CPU));
  uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU));
  lcd_init(LCD_DISP_ON);
  lcd_clrscr();
  lcd_puts_P("Brautomat v0.5\n");
  lcd_gotoxy(0,1);
  lcd_puts_P(__DATE__" aw");

  //3s Delay for Splash
  for(uint8_t i=0;i<60;++i)
    _delay_ms(15);
  lcd_clrscr();

  //H-Brücke
  DDRD |= _BV(PD2) | _BV(PD4) | _BV(PD5) | _BV(PD6) | _BV(PD7);
  //Relais für Heizung
  DDRB |= _BV(PB3);
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
  //Prescaler 128 = 125kHz ADC Clock, AutoTrigger, Interrupts enable
  ADCSRA = _BV(ADEN) | _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2) | _BV(ADATE) | _BV(ADSC) | _BV(ADIE); 

  //AVCC with external capacitor at AREF, internal 2.56V bandgap
  //siehe S. 215
  ADMUX = (_BV(REFS0) | _BV(REFS1)) + 2;   
  //ADC in Free Running mode
  SFIOR &= ~(_BV(ADTS2) | _BV(ADTS1) | _BV(ADTS0)); 

  //DS18B20 an PA6
  ow_set_bus(&PINA,&PORTA,&DDRA,PA6);

  //enable global interrupts
  sei();      

  for (;;)    /* main event loop */
    {
      processUART();
      if(do_ds18b20_meas)
      {
        //Temperatur am DS18B20 messen
        // zwischen DS18X20_start_meas und DS18X20_read_decicelsius_single
        // muß _delay_ms( DS18B20_TCONV_12BIT ); (750ms) gewartet werden.
        // alte Messung lesen
        int16_t decicelsius;
        DS18X20_read_decicelsius_single( gSensorID[0], &decicelsius );
        status.temperature = decicelsius/10.0;
        //neue Messung starten
        //DS18X20_start_meas( DS18X20_POWER_PARASITE, NULL );
        DS18X20_start_meas( DS18X20_POWER_EXTERN, NULL );
        do_ds18b20_meas = 0;
      }
      if(do_update_lcd)
      {
        update_lcd();
        do_update_lcd=0;
      }
    }
    return 0;
}
