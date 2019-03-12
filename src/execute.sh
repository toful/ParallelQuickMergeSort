#!/bin/bash

parts=(2 4 8 16 32 64 128)

rm time_$1.txt

for part in ${parts[@]}; do
	echo ${part}--------------
	echo ${part} "/////////////////////////////" >> time_$1.txt
    for i in {1..10}; do 
	    (/usr/bin/time -p ./$1 300000000 ${part} >> time_$1.txt) 2>> time_$1.txt
    done
done

exit 0