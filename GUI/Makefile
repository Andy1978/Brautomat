.PHONY = clean

LIBS:= `fltk-config --ldflags` `pkg-config --libs libconfig++`
CFLAGS:= -Wall -Wextra -ggdb `fltk-config --cxxflags` `pkg-config --cflags libconfig++`

all: brautomat_gui

brautomat_gui: brautomat_gui.cxx Spreadsheet.h
	$(MAKE) -C ../driver
	g++ $(CFLAGS) -o $@ ../driver/cBrautomat.o ../driver/serial.o $< $(LIBS)

brautomat_gui.cxx brautomat_gui.h: brautomat_gui.fld
	fluid -c $<

clean:
	rm -f brautomat_gui brautomat_gui.cxx brautomat_gui.h

brautomat_driver_test: brautomat_driver_test.o cBrautomat.o serial.o
	g++ $(CFLAGS) -o brautomat_driver_test brautomat_driver_test.o cBrautomat.o serial.o $(LIBS)
