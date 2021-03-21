#!/bin/bash
PARSER='./parser'
path="../Test"
MAKE=`make `
for i in `ls $path`
do
	t=`$PARSER $path/$i`
	echo $i
	mark=0
	for j in $t
	do
		if [ $j == 'Error' ]
		then
			if [ $mark == 0 ]
			then 
				mark=`expr $mark + 1`
			else
				echo ""
			fi
		fi
		echo -n "$j "
	done
	echo 
	echo
done
