#!/bin/bash

# Convert input arguments to uppercase
ARG1=$(echo "$1" | tr '[:lower:]' '[:upper:]')
ARG2=$(echo "$2" | tr '[:lower:]' '[:upper:]')
ARG3=$(echo "$3" | tr '[:lower:]' '[:upper:]')

#echo "Sending ${ARG2} command to ${ARG1}, with argument ${ARG3}"

if [ "${ARG2}" == "RPT" ]; then
	echo "Report on ${ARG1}'s MIB entry $ARG3 ..."
	./msei $ARG1 $ARG2 $ARG3 > tmp.txt
	sleep 1
	out=`./ms_mdre $ARG1 $ARG3 2>&1 | grep -v FATAL `
	while [ "${out}" == "" ]; do
		out=`./ms_mdre $ARG1 $ARG3 2>&1 | grep -v FATAL `
		sleep 1
	done
	echo "${out}"
else
	echo "Command $ARG1 to $ARG2 $ARG3 ..."
	./msei $ARG1 $ARG2 $ARG3 > tmp.txt
	sleep 3
	echo "${ARG1} replied ..."
	tail -n 1 mselog.txt
fi
rm -f tmp.txt
