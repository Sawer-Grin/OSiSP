#!/bin/bash
fileSave=$2
directory=$1

if [ "$#" != 2 ]; then 
	echo "problem with parametrs"
	echo "first parametr - directory for search"
	echo "second parametr - file for saving result"
	exit 0
fi 
if [[ ! -d "$directory" ]]; then 
	echo "Problem with file for saving"	
	echo "first parametr - directory for search"
	exit 0
fi

if [[ ! -f "$fileSave" ]]; then 
	echo "Problem with directory"	
	echo "second parametr - file for saving result"
	exit 0
fi


for i in $( find "$directory" -type d); do
	amountSize=$( du -b -h --summarize "$i" | cut -f1 )
	amountFiles=$( find "$i" -maxdepth 1 -type f | wc -l )
	echo "Directory: $i Size: $amountSize Value: $amountFiles" >> "$fileSave"

done

exit 0
