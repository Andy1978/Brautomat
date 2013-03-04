.PHONY = clean

brautomat_gui: brautomat_gui.fld
	fluid -c $<
	fltk-config --compile brautomat_gui.cxx

clean:
	rm -f brautomat_gui brautomat_gui.cxx brautomat_gui.h
