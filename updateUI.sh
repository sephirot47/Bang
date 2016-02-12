#!/bin/bash

cd res
for f in *.ui
do
	fname=$(echo $f | cut -d'.' -f1)
	uic $fname.ui > ../src/View/$fname.h
	echo "Created $fname.h" 
done
