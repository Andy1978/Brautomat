
  Buchstaben links in der unteren Zeile zeigen:
  Zeichen
  1: A = Temperaturegelung aktiv
     M = Heizung im manuellen(Hand-) Betrieb
  2: H = Heizung an
    ' '= Heizung aus
  3: S = Temperatursollwerte aus Schrittkette
    ' '=

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
  int8_t amplitude_set_point;  //Amplitude Rührwerk -127..126
  uint8_t period_set_point;     //Periodendauer Rührwerk in 100ms (0=keine Modulation)
  float step_temp[MAX_STEPS];   //Temperatur in der Schrittkette [°C]
  float dT_dt[MAX_STEPS];       //Temperaturanstieg [°C/min]
  uint16_t step_time[MAX_STEPS]; //Dauer Schritt [s]
  uint8_t	 bits;
  //Bit 0: Temperaturregelung aktiv (Handbetrieb wenn nicht)
  //Bit 1: Heizung aktiv im Handbetrieb
  //Bit 2: Temperatursollwerte aus Schrittkette
  //Bit 3: Schritt weiter (Flanke)
  //Bit 4: Schritt zurück (Flanke)
};
