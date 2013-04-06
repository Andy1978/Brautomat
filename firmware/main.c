/*************************************************

  Brautomat

  Autor: Andreas Weber 
  src: https://github.com/Andy1978/Brautomat
  changelog: 02.03.2013 angelegt
       06.04.2013 aw: PT100 nun über H-Brücke differentiell einlesen  

  Infos:  
  H-Brücken PWM Treiber IR2104
  IN obere Halbbrücke PD4, OC1B
  /SD obere Halbbrücke PD6
  IN untere Halbbrücke PD5, OC1A
  /SD untere Halbbrücke PD7
   
  Relais für Heizung: PB3

  PT100: differentiell Wheatstone bridge
  PDA0 : Spannungsteiler 6,8k, 100R von AREF (intern 2.56V bandgap)
  PDA1 : 6,8k, PT100 von AREF

  Dallas DS18xxx an PA6, externe Versorgung 4,7k Pull-Up

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

struct s_status
{
  float temperature;            //Isttemperatur [°C]
  uint8_t aktive_step;          //aktueller Schritt im Ablauf
  uint16_t remaining_step_time; //verbleibende Zeit im aktuellen Schritt [s]
  uint8_t  bits;
  //Bit 0 H: Heizung aktiv
};

struct s_setvalues
{
  float temperature_set_point;  //Solltemperatur [°C]
  uint8_t amplitude_set_point;  //Amplitude Rührwerk 0-255
  uint8_t period_set_point;     //Periodendauer Rührwerk in 100ms (0=keine Modulation)
  float step_temp[5];           //Temperatur in der Schrittkette
  uint16_t step_time[5];        //Dauer Schritt
  uint8_t  bits;
  //Bit 0 A: Temperaturregelung aktiv (Handbetrieb wenn nicht)
  //Bit 1 M: Heizung aktiv im Handbetrieb
  //Bit 2 S: Temperatursollwerte aus Schrittkette
  //Bit 3: Schritt weiter (Flanke)
  //Bit 4: Schritt zurück (Flanke)
};

#define UART_BAUD_RATE 57600
#define MAXSENSORS 5
#define NEWLINESTR "\r\n"
#define OW_ONE_BUS

volatile struct s_status status;
volatile struct s_setvalues setvalues;

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
/*
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
*/



  dtostrf(status.temperature,4,1,buf);
  lcd_gotoxy(0,0);
  _delay_ms(2);
  //itoa(tmp,buf,10);
  lcd_puts(buf);
  lcd_puts(" ");

  uint8_t i = gSensorID[0]; // family-code for conversion-routine
  int16_t decicelsius;
  DS18X20_start_meas( DS18X20_POWER_PARASITE, NULL );
  _delay_ms( DS18B20_TCONV_12BIT );
  DS18X20_read_decicelsius_single( i, &decicelsius );
  DS18X20_format_from_decicelsius( decicelsius, buf, 10 );
  lcd_puts(buf);
  lcd_puts("    ");
}


ISR(TIMER0_COMP_vect) //1kHz
{
  static uint8_t disp_cnt=0;
    
  if (!disp_cnt++)
    update_lcd();
}

int8_t sin_list[]={0,24,48,70,89,105,117,124,127,124,117,105,89,70,48,24,0,-24,-48,-70,-89,-105,-117,-124,-127,-124,-117,-105,-89,-70,-48,-24};

ISR(ADC_vect) 
{
  //Isttemperatur mal über Poti  
  //status.temperature=ADC/1024.0*100;
  //int16_t temp=ADC-512;
  //OCR1A=512-temp;
  //OCR1B=512+temp;

  int16_t tmp=ADC;
  if(tmp>511) tmp = tmp-1024;
  status.temperature=tmp/10.0;        

/*
  static float i=0;
  i+=ADC/50000.0;
  if (i>32) i-=32;
  OCR1A=512-sin_list[(uint8_t)i];
  OCR1B=512+sin_list[(uint8_t)i];
*/  
}

// UART bearbeiten. Es gibt nur ein Telegramm mit allen Sollwerten
// und eine Antwort mit allen Istwerten bzw. Status
void processUART(void)
{
  //Alle Daten empfangen
  //momentan keine Fehlerbehandlung
  if(uart_GetRXCount()>=sizeof(struct s_setvalues))
  {
    //Empfangen
    char* rec=(char*)&setvalues;
    uint8_t i;
    for(i=0;i<sizeof(struct s_setvalues);i++)
    {
      rec[i]=uart_getc() & 0xFF;
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
  uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(UART_BAUD_RATE,F_CPU));
  lcd_init(LCD_DISP_ON);
  lcd_clrscr();
  lcd_puts_P("Brautomat v0.2\n");
  lcd_gotoxy(0,1);
  lcd_puts_P(__DATE__" aw");

  //3s Delay for Splash
  for(uint8_t i=0;i<20;++i)
    _delay_ms(15);
  lcd_clrscr();

  //H-Brücke
  DDRD |= _BV(PD4) | _BV(PD5) | _BV(PD6) | _BV(PD7);
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
  //Prescaler 64 = 250Khz ADC Clock, AutoTrigger, Interrupts enable
  ADCSRA = _BV(ADEN) | _BV(ADPS1) | _BV(ADPS2) | _BV(ADATE) | _BV(ADSC) | _BV(ADIE); 
  //AVCC with external capacitor at AREF pin als Reference verwenden
  //siehe S. 215
  //8=Multiplexer ADC0 positive Input, ADC0 negative, 10x gain
  //9=Multiplexer ADC1 positive Input, ADC0 negative, 10x gain

  //Channel 9 
  ADMUX = _BV(REFS1) | _BV(REFS0) | 11;   
  //ADC in Free Running mode
  SFIOR &= ~(_BV(ADTS2) | _BV(ADTS1) | _BV(ADTS0)); 

  //DS18B20
  ow_set_bus(&PINA,&PORTA,&DDRA,PA6);

  //enable global interrupts
    sei();      
  
    for (;;)    /* main event loop */
    {
      processUART();
    }
    return 0;
}
