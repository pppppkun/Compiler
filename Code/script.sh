#!/bin/bash
PARSER='./parser'
path="../Test"
for i in `ls $path`
do
	t=`$PARSER $path/$i`
	mark=0
	for j in $t
	do
		mark=`expr $mark + 1`
		echo -n "$j "
		if [ $mark == 10 ]
		then
			echo ""
			mark=`expr $mark - 10`
		fi
	done
done
