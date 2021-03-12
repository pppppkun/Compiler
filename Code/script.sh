#!/bin/bash
read -p "Enter Test Path: " path
echo $path
SCAN='./scanner'
`make scanner`
for i in `ls $path`
do
	t=`$SCAN $path$i`
	echo $t
done
