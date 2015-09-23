#!/bin/bash

#echo Sending $2 command to $1, with argument $3 

if [ $2 == "RPT" ]; then
	echo Report on $1s MIB entry $3 ...
	./msei $1 $2 $3 > tmp.txt
	sleep 1
	out=`./ms_mdre $1 $3 2>&1 | grep -v FATAL `
	while [ "${out}" == "" ]; do
		out=`./ms_mdre $1 $3 2>&1 | grep -v FATAL `
		sleep 1
	done
	echo "${out}"
else
	echo Command $1 to $2 $3 ...
	./msei $1 $2 $3 > tmp.txt
	sleep 3
	echo $1 replied ...
	tail -n 1 mselog.txt
fi
rm -f tmp.txt
