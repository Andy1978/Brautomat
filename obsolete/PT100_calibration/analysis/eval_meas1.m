## 18.4.2013 Andreas Weber

## erste Messung ohne timestamp und int16 overflow
d=load("../captures/100deg_1.log.gz");

ds=d(:,1);
pt100=d(:,2);

ovf_corr=zeros(rows(pt100),1);
ovf_corr(find(diff(pt100)>200)+1)-=256;
ovf_corr(find(diff(pt100)<-200)+1)+=256;
pt100 += cumsum(ovf_corr) -256;

R1=R2=6800;
R3=120; Gain=200;

T=adc2t (R1, R2, R3, pt100, Gain);

plot(ds,";DS18B20 [C];",pt100,";PT100 ADC Raw (int16 ovf fixed);",T,";PT100 uncal. [C];");
grid on

disp("old archive measurement. This showed two problems:")
disp("int16 overflow in summation in AVR firmware and")
disp("bigger difference at lower temperatures.")
