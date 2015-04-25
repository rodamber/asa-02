#!/bin/sh

V=10
E=20
N=500

echo N=$N

echo Removing tests...
rm ../graphs/in/*.in
rm ../graphs/out/*.out

echo Generating new tests...

for i in `seq 1 $N`
do
    ./p2-gen -v:$V -e:$E -c:0.95 -minw:0 -maxw:50 -nc:0 -o:t$i.in
    V=`expr $V + 100`
    E=`expr 2 \* $V`
    ../../src/a.out < t$i.in > ../graphs/out/t$i.out
    mv t$i.in ../graphs/in
done

echo Done.
