## 27.10.2013 Andreas Weber
## Aufbau:
## second, usec since epoch, Isttemperatur, Solltemperatur, aktiver Schritt,
## Zeit verbleibend, Status Heizung

#d = load("cBrautomat_status.log");
#d = load("efbe_schott_wasserkocher.log");

## 12l Wasser, erster Test am 5.11.2013
#d = load("Wasser12l.log");

d = load("cBrautomat_status.log");

t = d(:,1)+d(:,2)/1e6;
t -= t(1);
T_ist = d(:,3);
T_soll = d(:,4);
active_step = d(:,5);
time_remaining = d(:,6);
state_heater = d(:,7);

plot(t, T_ist, ";Ist;", t, T_soll, ";Soll;", t, state_heater*10,";Heizung;")
grid on
xlabel("Zeit [s]");
ylabel("Temperatur [C]");
legend("location","east")

#title("Test Nr.1 mit 12l Wasser")
#print("test1_12l_wasser.png")
