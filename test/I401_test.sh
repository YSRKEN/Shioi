#!/bin/bash
function simple_test(){
	echo "I401.out $2 $3 $4"
	./I401.out $2 $3 $4 --debug
	local re=$?
	echo "return pos:"$re
	if [ $1 -ne $re ] ; then
		echo $1
		echo "test fail"
		exit 1
	fi
	return 0
}

# main

pwd
ls
cd I401
pwd
ls
simple_test 99 NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNBNNNNNNNNNNNNNNWBBWWNNNNNNNNNNNBNBNNNNNNNNNNNWWBNWNNNNNNNNNNNBNBNNNNNNNNNNNWNNNWNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN B 0
echo "test finish."
