#!/bin/bash

exec &> ../data/times.csv

for file in ../graphs/in/*
do
    exec < $file

    read V E

    echo -n `expr $V \* $E`
    echo -n ";"

    TIMEFORMAT=%R
    time ../../src/a.out < $file > /dev/null
done
