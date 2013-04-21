## 19.4.2013 Andreas Weber
## little timer helper script for manual DS150 captures

h=zenity_progress("waiting..");
for(k=1:60)
beep();
fflush(stdout);
for(i=1:100)
  sleep(3)
  zenity_progress(h,i);
endfor
endfor
