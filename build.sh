#!/bin/bash

for textfile in $( ls . | grep .cpp$ | grep -v ^out_ ); do
	echo -e "\033[92;1mbuilding ${textfile} ...\033[0m"
	g++ -std=c++17 -pthread -Ofast "${textfile}" -o "out_${textfile}"
done

