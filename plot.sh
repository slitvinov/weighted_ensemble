for i in *.[ab].dat
do
    o=`basename "$i" .dat`.png
    gnuplot < "$i" > "$o" -e '
set term pngcairo size 1080, 1080 font "Helvetica,30" linewidth 5;
unset key;
set xlabel "position";
set ylabel "ID";
set ytics 250;
plot [-0.05:0.5][-90:500] "-" u 3:1:(sqrt($4)/4) w circle fs transparent solid 0.1  border lt -1
' || exit 2
done
