#!/bin/bash
PARSER='./parser'
path="../Test_Advance"
MAKE=`make `
output_file='../test_advance.txt'
result_file='../result_advance.txt'
CMM='.cmm'
OUTPUT='.output'
echo > $output_file
echo > $result_file
for i in `find $path -name "*.cmm" | sed 's/.cmm//g' | sed  's/..\/Test_Advance\///g'`
do
	
	echo $i$CMM >> $output_file
	echo $i$OUTPUT >> $result_file
	cat $path/$i$OUTPUT >> $result_file
	t=`$PARSER $path/$i$CMM >> $output_file`
done
