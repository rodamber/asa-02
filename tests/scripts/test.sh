#!/bin/sh

echo "Testing all tests"

make -C ../../src;

echo "**************************************************"
for i in ../graphs/in/*;
    do echo "=== Testing $i";
    cat $i | ../../src/a.out > /tmp/`basename $i .in`.myout;
    #diff /tmp/`basename $i .in`.myout ../graphs/out/`basename $i .in`.out;
    rm /tmp/`basename $i .in`.myout
done
echo "**************************************************"

echo "Done."
