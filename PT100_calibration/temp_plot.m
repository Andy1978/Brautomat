## 18.4.2013 Andreas Weber
## Auswertung der Temperatur logfiles

##                               ___    |
##                           5V -UUU----oAVcc
##                                      |
##  .----o--------------o---------------oARef
##  |    |              |               |
##  |   .-.            .-.              |
##  |   | | R1         | | R2           |
##  |   | | 6.8k       | | 6.8k         |
##  |   '-'            '-'              |
##  |    |              |               |
##  |    o--------------)---------------oADC 0
## ---   |              |               |
## ---   |              o---------------oADC 1
##  |    |              |               |
## 100nF |              |               |
##  |   .-.            .-.              |
##  |   | | Rpt        | | R3           |
##  |   | | PT100      | | 120R         |
##  |   '-'            '-'              |
##  |    |              |               |
##  '----o--------------o---------------oAGND
##                                      |
##(created by AACircuit v1.28.6 beta 04/19/05 www.tech-chat.de)

## erste Messung ohne timestamp und int16 overflow
## d=load("100deg_1.log.gz");
## plot(d(:,1),";DS18B20;",d(:,2),";PT100 ADC Raw;");

## zweite Messung von 95°C bis -5°C
d=load("100deg_2.log");

R1=R2=6800;
R3=120;
Gain=200;
R0=100;

## Ud = ARef*(Rpt/(R1+Rpt)-R3/(R2+R3)) Differenzspannung Wheatstone Brücke ADC0-ADC1
## ADC = Gain/ARef*512*Ud
## PT100 Polynom
a = 3.9083e-3;   #1 / °C
b = -5.775e-7;   #1 / °C^2

ti=linspace(0,100,1000);
Ri=R0*polyval([b a 1],ti);
ADC=Gain*512*(Ri./(R1+Ri)-R3/(R2+R3));
t=interp1(ADC,ti,d(:,3));
plot(d(:,2),";DS18B20;",t,";uncal. PT100 Temp;");
grid on
