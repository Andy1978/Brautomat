## 3.3.2013 Andreas Weber Makefile für den Brautomat Treiber

.PHONY: clean

LIBS:= `fltk-config --ldflags` `pkg-config --libs libconfig++`
CFLAGS:= -Wall -Wextra -ggdb `pkg-config --cflags libconfig++`

brautomat_driver_test: brautomat_driver_test.o cBrautomat.o serial.o
	g++ $(CFLAGS) -o brautomat_driver_test brautomat_driver_test.o cBrautomat.o serial.o $(LIBS)

%.o: %.cpp %.h
	g++ $(CFLAGS) -c -o $@ $<


clean:
	rm -f *.o brautomat_driver_test
	find . -name "octave-workspace" -exec rm -f {} \;
	find . \( -name "*.m" -or -name "*.c*" -or -name "*.h" -or -name "*.f" \) -exec sed -i 's/[[:space:]]*$$//' {} \;
