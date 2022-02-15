#!/bin/bash

for i in $( find "$1" -type d); do
	mountainside=$( du -h --max-depth=0 "$i" | cut -f1 )
	amountFiles=$( ls "$i" -F | grep -v / | wc -l )
	echo "Directory: $i Size: $amountSize Value of files:$amountFiles" >> "$2"

done

exit 0
