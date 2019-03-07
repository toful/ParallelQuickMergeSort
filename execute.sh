#!/bin/bash

parts=(2 4 8 16 32 64 128)
sizes=(120000000 180000000 240000000 360000000) 

for size in ${sizes[@]}; do
	echo --------------------${size}-----------------
	echo --------------------${size}----------------- >> time_$1.txt
	for part in ${parts[@]}; do
		echo --------------${part}--------------
		echo --------------${part}-------------- >> time_$1.txt
		(/usr/bin/time -p ./$1 ${size} ${part} >> time_$1.txt) 2>> time_$1.txt
	done
done
exit 0