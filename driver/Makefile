## 3.3.2013 Andreas Weber Makefile für den Brautomat Treiber

.PHONY: clean

LIBS:= `pkg-config --libs libconfig++`
CFLAGS:= -Wall -Wextra -ggdb `pkg-config --cflags libconfig++`

brautomat_driver_test: brautomat_driver_test.cpp cBrautomat.o serial.o
	g++ $(CFLAGS) -o brautomat_driver_test brautomat_driver_test.cpp cBrautomat.o serial.o $(LIBS)

%.o: %.cpp %.h
	g++ $(CFLAGS) -c $<


clean:
	rm -f *.o brautomat_driver_test *.log
