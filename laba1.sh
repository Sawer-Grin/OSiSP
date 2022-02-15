#!/bin/bash

for i in $( find "$1" -type d); do
	amountSize=$( du --max-depth=0 "$i" | cut -f1 )
	amountFiles=$( find "$i" -maxdepth 1 -type f | wc -l )
	echo "Directory: $i Size: $amountSize Value of files:$amountFiles" >> "$2"

done

exit 0
