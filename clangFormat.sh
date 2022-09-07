#!/bin/bash

for i in `find $Dir -regex '.*/.*\.\(c\|cpp\|h\|hpp\|\tcc\)$'`
do
    echo "Perfoming format on $i"
    clang-format $i --style="file" > $i.clang
    mv $i.clang $i
done

