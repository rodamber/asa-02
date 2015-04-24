#!/bin/bash

exec &> times.csv

for file in graphs/in/*
do
    exec < $file

    read V E

    echo -n `expr $V \* $E`
    echo -n ";"

    TIMEFORMAT=%R
    time ../a.out < $file > /dev/null
done
