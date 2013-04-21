## 21.4.2013 Andreas Weber

.PHONY: clean

plots3 = meas3_diff_DS18B20_D150.png meas3_DS18B20_uncal_PT100_D150.png meas3_DS18B20_D150.png meas3_hist_diff_DS18B20_D150.png

$(plots3): eval_meas3.m ../captures/100deg_3.log ../captures/huber_D150_3.log adc2t.m
	octave -q eval_meas3.m

clean:
	rm -f *.png
	rm -f octave-workspace
