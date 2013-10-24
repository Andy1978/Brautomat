.PHONY:all
.PHONY:clean

all:
	$(MAKE) -C firmware
	$(MAKE) -C driver
	$(MAKE) -C GUI
	$(MAKE) -C PT100_calibration

clean:
	$(MAKE) -C firmware clean
	$(MAKE) -C driver clean
	$(MAKE) -C GUI clean
	$(MAKE) -C PT100_calibration clean
	find . -name "octave-workspace" -exec rm -f {} \;
	find . \( -name "*.m" -or -name "*.c*" -or -name "*.h" -or -name "*.f" \) -exec sed -i 's/[[:space:]]*$$//' {} \;
