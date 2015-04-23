#!/bin/sh

echo "Testing all tests"

make -C ..;

echo "**************************************************"
for i in *.in;
    do echo "=== Testing $i";
    cat $i | ../a.out > myout.d/`basename $i .in`.myout;
    diff myout.d/`basename $i .in`.myout `basename $i .in`.out;
done
echo "**************************************************"

echo "Done."
