## 18.4.2013 Andreas Weber
## Auswertung der Temperatur logfiles

## zweite Messung von 95°C bis -5°C
d=load("../captures/100deg_2.log");

## Von Hand aufgezeichnete Messung mit "huber precision thermometer D150"

## Timestamp in HH:MM:SS format TODO: implement me!
#dm=load("../captures/huber_D150_2.log");

plot(d(:,2),";DS18B20;"), grid on
