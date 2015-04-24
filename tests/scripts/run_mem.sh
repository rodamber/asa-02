#!/bin/bash

exec &> mems.csv

for file in graphs/in/*
do
    exec < $file

    read V E

    echo -n `expr $V \* $E`
    echo -n ";"
    valgrind --log-file="val.out" ../a.out < $file > /dev/null
    head -n 10 val.out | tail -1 | cut -d " " -f 11
done
