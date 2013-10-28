.PHONY = clean

LIBS:= `fltk-config --ldflags` `pkg-config --libs libconfig++`
CFLAGS:= -Wall -Wextra -ggdb `fltk-config --cxxflags` `pkg-config --cflags libconfig++`

all: brautomat_gui

cBrautomat.o: ../driver/cBrautomat.cpp ../driver/cBrautomat.h
	g++ $(CFLAGS) -c $< $(LIBS)

serial.o: ../driver/serial.cpp ../driver/serial.h
	g++ $(CFLAGS) -c $< $(LIBS)

brautomat_gui: brautomat_gui.cxx cBrautomat.o serial.o Spreadsheet.h ../driver/cBrautomat.h
	g++ $(CFLAGS) -o $@ cBrautomat.o serial.o $< $(LIBS)

brautomat_gui.cxx brautomat_gui.h: brautomat_gui.fld
	fluid -c $<

clean:
	rm -f *.o brautomat_gui brautomat_gui.cxx brautomat_gui.h

