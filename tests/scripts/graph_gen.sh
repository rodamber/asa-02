#!/bin/sh

V=10
E=20
N=300

echo N=$N

echo Removing tests...
rm t*.in

echo Generating new tests...

for i in `seq 1 $N`
do
    ./p2-gen -v:$V -e:$E -c:0.95 -minw:0 -maxw:50 -nc:0 -o:t$i.in
    mv t$i.in graphs/in
    V=`expr $V + 100`
    E=`expr 2 \* $V`
done

echo Done.
