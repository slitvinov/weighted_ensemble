c=1
for i in *.[ab].dat
do
    b=`awk '2 * NR >= '$c'{print "\"\" " $2; exit}' b`
    o=`basename "$i" .dat`.png
    gnuplot < "$i" > "$o" -e '
set term pngcairo size 1080, 1080 font "Helvetica,30" linewidth 5;
unset key;
unset ytics;
set xlabel "position";
set xtics ('"$b"');
set grid xtics linestyle -1;
plot [-0.05:0.55][-0.5:7.5] "-" u 2:1:(0.025) w circle fs transparent solid 0.1  border lt -1
' || exit 2
    c=$(($c+1))
done
