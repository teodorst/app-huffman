#!/bin/bash

for i in 10 20 30 40 50 100 200 300 400
do
	printf "Computing time for a $i MB file...\n"
	printf "\nTime for $i MB file:\n" >> timeOpenMP.txt
	filename='M.in'
	filename=$i$filename
	{ time ./huffman_OpenMP compress tests/$filename tests/$iM.out tests/$iM.codif ; } 2>> timeOpenMP.txt
done	
