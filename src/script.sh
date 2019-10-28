#!/bin/bash

templine=""

# check all log files #
for file in log/Worker*
do
	while read line;  # for each line 
	do	
		# keep only search results, add the whole command in string #
  		templine=`awk '{ if($3 == "/search:") {print}} ' <<< $line | awk '{print}'`
		words=`(echo $templine | wc -w)` # get number of words
		
		wordsNum=$((words-4)) # calculate number of paths(words - (date,time,command,word))

		# write to file the word, times the number of paths #
		for i in $(seq 1 $wordsNum);
		do
			temp=`echo $templine | awk '{print $4}'`
			echo $temp >> tempfile 
		done
	done < $file
done

# keep to another file all the unique words #
grep -o -E '\w+' tempfile | sort -u -f >> tempfile1

# remove : from file #
cat tempfile | tr -d ':' >> tempfile2

totalKeyWords=`cat tempfile1 | wc -l` # total keyWords searched #
minTimes=9999999
minWord=""
maxTimes=0
maxWord=""

# for every unique line
while read l;
do
	# find how many times it is repeated in file #
	currT=`grep -w $l tempfile2 | wc -l`
	currTimes=$((currT))

	if [ $currTimes -gt $maxTimes ]
	then
		maxTimes=$currTimes
		maxWord=$l
	fi

	if [ $currTimes -lt $minTimes ]
	then
		minTimes=$currTimes
		minWord=$l
	fi
done <tempfile1

bold=$(tput bold)
normal=$(tput sgr0)

echo "Total number of keywords searched: ${bold}$totalKeyWords${normal}"
echo Keyword most frequently found: ${bold}$maxWord${normal} [totalNumFilesFound: ${bold}$maxTimes${normal}]
echo Keyword least frequently found: ${bold}$minWord${normal} [totalNumFilesFound: ${bold}$minTimes${normal}]


rm tempfile tempfile1 tempfile2
