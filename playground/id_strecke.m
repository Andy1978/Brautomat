## 11.11.2013 Andreas Weber
## Streckenidentifikation

d=load("../GUI/Wasser12l.log");

t = d(:,1)+d(:,2)/1e6;
t -= t(1);
T_ist = d(:,3);
state_heater = d(:,7);
#plot(t, T_ist,t,state_heater*30+20)

## interpolate äquidistant
T=0.5;
ti=0:T:max(t);
T_ist_i = interp1(t,T_ist,ti);
state_heater_i = interp1(t,state_heater,ti,"nearest");
clear d t T_ist state_heater;

#plot(ti,T_ist_i,ti,state_heater_i*30+20)

pkg load control
#dat = iddata(T_ist_i', state_heater_i',T);
#sys=arx(dat,2)
#[sys, X0, INFO] = moen4(dat,2);
#step(tf(sys),100)
#[Y, T, X] = lsim (sys, state_heater_i, ti);
#plot(T,Y)


## manuell

s=tf([0.0302], [90, 1, 0.00011]);
[yo,to,xo] = lsim(s, state_heater_i, ti);
yo+=T_ist_i(1);

plot(ti,T_ist_i,ti,state_heater_i*30+20,ti, yo)


### optimieren mit sqp
num = cell2mat(get(s,"num"));
den = cell2mat(get(s,"den"));

function ret = delta(X, t, T_ist, heater)
  s=tf(X(1),X(2:end));
  [yo,to,xo] = lsim(s, heater, t);
  yo+=T_ist(1);
  #keyboard
  ret=sumsq(yo-T_ist');
  
endfunction

f = @(X) delta(X, ti, T_ist_i, state_heater_i)

X0 = [num den]
f(X0)
#[X, OBJ, INFO, ITER, NF, LAMBDA] = sqp (X0, f)
