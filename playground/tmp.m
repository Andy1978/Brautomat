pkg load control

L = tf(3e4 * [0.0025 0.1 1], [0.01 1.03 3.03 3.01 1]);
rlocus(L) #Fehler roots: inputs must not contain Inf or NaN, warum?

##filt: ähnliche Funktion wie im signal package
## dort kann man sich z.B. b und a Koeffizienten für einen Filter
## berechnen lassen

[b,a]=butter(3,0.5)
freqz(b,a)

#nun mit filt die Übertragungsfunktion
S = filt(b,a)
bode(S)
