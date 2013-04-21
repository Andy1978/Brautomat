## 19.4.2013
## Helper script for Brautomat/PT100_calibration.
## Converts the raw ADC value to temperature.
## See Readme for ADC connection circuit

function T = adc2t (R1, R2, R3, R4, Gain)

  R0=100;
  ## Ud = ARef*(Rpt/(R1+Rpt)-R3/(R2+R3)) Differenzspannung Wheatstone Brücke ADC0-ADC1
  ## ADC = Gain/ARef*512*Ud
  ## PT100 Polynom
  a = 3.9083e-3;   #1 / °C
  b = -5.775e-7;   #1 / °C^2

  ti  = linspace (-20, 120, 200);
  Ri  = R0 * polyval ([b a 1], ti);
  ADC = Gain * 512 * (Ri./(R1+Ri)-R3/(R2+R3));
  T   = interp1 (ADC, ti, R4, "extrap");
endfunction
