## 3.3.2013 Andreas Weber Makefile für den Brautomat Treiber

.PHONY: clean
CC_FLAGS := -Wall -Wextra -ggdb -O0

brautomat_driver_test: brautomat_driver_test.o cBrautomat.o serial.o
	g++ $(CC_FLAGS) -o brautomat_driver_test brautomat_driver_test.o cBrautomat.o serial.o

%.o: %.cpp %.h
	g++ $(CC_FLAGS) -c -o $@ $<


clean:
	rm -f *.o brautomat_driver_test
