#!/bin/bash

set ret=;

ret=`aplay -l | awk 'NR==2' | awk '{print $10}'`;

if [ $ret == "USB" ]
then
	exit 10;
else
	exit 11;
fi
