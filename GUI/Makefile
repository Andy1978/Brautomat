.PHONY = clean

brautomat_gui: brautomat_gui.fld Spreadsheet.h
	fluid -c $<
	fltk-config --compile brautomat_gui.cxx

clean:
	rm -f brautomat_gui brautomat_gui.cxx brautomat_gui.h
