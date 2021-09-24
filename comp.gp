### ./main -n 20 -b 10 -t 0.00001 | tee data/n20b10
set term pngcairo size 1080, 1080 font "Helvetica,30" linewidth 5;
F = 3.553304556611753E-5
set log x
set key center
set xlabel "time"
set ylabel "cumulative flux / time"
set ytics 2e-5
plot [10:][0:] \
     F w l lw 3 lt -1 t "exact", \
     "data/n10b10" u ($2/$1) w lp t "n = 20, b = 10", \
     "data/n20b10" u ($2/$1) w lp t "n = 20, b = 10", \
     "data/n10b100" u ($2/$1) w lp t "n = 10, b = 100", \
     "data/n20b100" u ($2/$1) w lp t "n = 20, b = 100"
     
