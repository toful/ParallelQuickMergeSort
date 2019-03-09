#!/bin/bash

rm $1.out
Ncol=$2
timeValues=$(egrep "real" $1 | cut -d ' ' -f2)
i=0
for value in ${timeValues[@]}
do
    if [ $i -eq $Ncol ]
    then
        i=0
        echo "" >> $1.out
    fi
    echo -n $value", " >> $1.out
    ((i++))
done

exit 0