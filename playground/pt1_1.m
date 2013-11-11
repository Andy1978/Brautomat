pkg load control
T1=0.4;               # time constant
P=tf([1], [T1 1])     # create transfer function model
step(P,2)             # plot step response for the first 2 seconds

#add some common markers like tangent at the origo which crosses lim(n->inf) f(t) at t=T1
hold on
plot ([0 T1],  [0 1],"g")
plot ([T1 T1],  [0 1],"k")
plot ([0 T1],  [1-1/e 1-1/e],"m")
hold off

figure
bode(P)
