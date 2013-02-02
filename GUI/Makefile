.PHONY = clean

gui: gui.fld
	fluid -c gui.fld
	fltk-config --compile gui.cxx

clean:
	rm -f gui gui.cxx gui.h
