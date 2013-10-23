## 3.3.2013 Andreas Weber Makefile für den Brautomat Treiber

.PHONY: clean
CC_FLAGS := -Wall -Wextra -ggdb -O0

brautomat_driver_test: brautomat_driver_test.o cBrautomat.o serial.o
	g++ $(CC_FLAGS) -o brautomat_driver_test brautomat_driver_test.o cBrautomat.o serial.o

%.o: %.cpp %.h
	g++ $(CC_FLAGS) -c -o $@ $<


clean:
	rm -f *.o brautomat_driver_test

#~ .PHONY:clean
#~ .PHONY:all
#~ 
#~ ifeq ($(shell uname),MINGW32_NT-5.1)
  #~ LIBS:= `fltk-config --ldflags` -L/local/lib -lconfig++
  #~ CFLAGS:= `fltk-config --cxxflags` -Wall -I/local/include
#~ else
  #~ LIBS:= `fltk-config --ldflags` `pkg-config --libs libconfig++`
  #~ CFLAGS:= `fltk-config --cxxflags` `pkg-config --cflags libconfig++`
#~ endif
#~ 
#~ TARGET := pattern_cfg_editor
#~ 
#~ all: $(TARGET)
	#~ @echo "LIBS = $(LIBS)"
	#~ @echo "CFLAGS = $(CFLAGS)"	
#~ 
#~ %.o: %.c
	#~ g++ -Wall $(CFLAGS) -c $<
#~ 
#~ $(TARGET): $(TARGET).cxx table.h
	#~ g++ -ggdb -Wall $(CFLAGS) $^ $(LIBS) -o $@
#~ 
#~ $(TARGET).cxx: $(TARGET).f
	#~ fluid -c $^
#~ 
#~ clean:
	#~ rm -f *.o  $(TARGET).exe $(TARGET) $(TARGET).cxx $(TARGET).h
