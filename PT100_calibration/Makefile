## 8.4.2013 Andreas Weber Makefile für PT100 Kalibration

.PHONY: clean
CC_FLAGS := -Wall -Wextra -ggdb -O0

pt100cal: pt100cal.o ../driver/cBrautomat.o ../driver/serial.o
	g++ $(CC_FLAGS) -o $@ pt100cal.o ../driver/cBrautomat.o ../driver/serial.o

../driver/cBrautomat.o: ../driver/cBrautomat.cpp ../driver/cBrautomat.h
	$(MAKE) -C ../driver

../driver/serial.o: ../driver/serial.cpp ../driver/serial.h
	$(MAKE) -C ../driver

clean:
	rm -f *.o pt100cal
