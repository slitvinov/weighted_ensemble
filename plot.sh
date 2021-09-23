for i in *.[ab].dat
do
    o=`basename "$i" .dat`.png
    GNUTERM=pngcairo gnuplot < "$i" > "$o" -e '
unset key;
set xlabel "position";
set ylabel "ID";
plot [-0.05:0.5][-90:800] "-" u 3:1:(sqrt($4)/4) w circle fs transparent solid 0.1  border lt -1 lw 2
'
done
