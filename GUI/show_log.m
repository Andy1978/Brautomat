## 27.10.2013 Andreas Weber
## Aufbau:
## second, usec since epoch, Isttemperatur, Solltemperatur, aktiver Schritt,
## Zeit verbleibend, Status Heizung

d = load("cBrautomat_status.log");
#d = load("efbe_schott_wasserkocher.log");

t = d(:,1)+d(:,2)/1e6;
t -= t(1);
T_ist = d(:,3);
T_soll = d(:,4);
active_step = d(:,5);
time_remaining = d(:,6);
state_heater = d(:,7);

plot(t, T_ist, ";Isttemperatur;", t, T_soll, ";Solltemperatur;", t, state_heater)
grid on
xlabel("Zeit [s]");
ylabel("Temperatur [C]");
