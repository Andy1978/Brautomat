## 19.4.2013 Andreas Weber
## Auswertung der Temperatur logfiles
## See Readme for circuit

## dritte Messung (lang), zuerst isolierter Becher, dann Kaffeetasse
## von 95°C bis 13.3°C
d=load("../captures/100deg_3.log");
R4=d(:,3);

## Von Hand aufgezeichnete Messung mit "huber precision thermometer D150" 
dm=load("../captures/huber_D150_3.log");

Rl=0; #Leitungswiderstand
R1=R2=6800;
R3=120; Gain=200;
#R3=119.71; Gain=200.08;

T=adc2t (R1, R2, R3, R4-Rl, Gain);

tstart = d(1,1); #start measurement
plot(d(:,1)-tstart,d(:,2),";DS18B20;",d(:,1)-tstart,T,";uncal. PT100 Temp;",dm(:,1)-tstart,dm(:,2),"-x;manual DS150;");
ylabel("Temperature [C]");
xlabel("Time [s]")
grid on
print("meas3_DS18B20_uncal_PT100_D150.png");

## Versuch R1, R2, R3 und Gain so zu optimieren, daß die Abweichung zum DS18B20 minimal wird

ind=[406:7800 7861:7973 8021:9270] ;

opt=0;
ds=d(ind,2);
pt100=d(ind,3);

if (opt)
  plot(d(:,2),";DS18B20;",T,";uncal. PT100 Temp;");
  #X0=[R1, R2, R3, Gain];
  X0=[Rl, R3, Gain];
  #f=@(x) sumsq(abs(ds-adc2t(x(1),x(2),x(3),pt100,x(4))))
  f=@(x) sumsq(abs(ds-adc2t(R1,R2,x(2),pt100-x(1),x(3))))
  [X, OBJ, INFO, ITER, NF, LAMBDA] = sqp (X0, f)
  
  T=adc2t (R1, R2, X0(2), R4-X0(1), X0(3));
  plot(d(:,1)-tstart,d(:,2),";DS18B20;",d(:,1)-tstart,T,";uncal. PT100 Temp;",dm(:,1)-tstart,dm(:,2),"-x;manual DS150;");
endif


## Statistiken
## DS150 und DS18B20 im Zeitraum (Anfang 1:7 weggeschnitten wegen unterschiedlicher Zeitkonstante)
ind=8:length(dm);
t=dm(ind,1);
ds_interp=interp1(d(:,1),d(:,2),t);
plot(t-tstart,ds_interp,"-x;DS18B20;",t-tstart,dm(ind,2),"-+;DS150;")
grid on
print("meas3_DS18B20_D150.png");

plot(t-tstart,ds_interp-dm(ind,2),"-x;DS18B20 - DS150;")
grid on
print("meas3_diff_DS18B20_D150.png");

## Mittlere Abweichung
d=ds_interp-dm(ind,2);

printf ("mean abs(difference) = %f °C\n", mean(abs(d)))
printf ("min  difference = %f °C\n", min(d))
printf ("max  difference = %f °C\n", max(d))

hist(d,-0.2:0.1:0.6)
grid on
title("histogram DS18B20 - D150(reference)")
xlabel("Temperature difference [C]");
ylabel("count")
print("meas3_hist_diff_DS18B20_D150.png");
