#!/bin/sh

echo "Testing all tests"

make -C ..;

echo "**************************************************"
for i in prof/*.in;
    do echo "=== Testing $i";
    cat $i | ../a.out > myout.d/`basename $i .in`.myout;
    diff myout.d/`basename $i .in`.myout prof/`basename $i .in`.out;
done
echo "**************************************************"

echo "Done."
