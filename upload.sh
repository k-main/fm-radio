#!/bin/bash

arduino-cli compile --fqbn arduino:avr:nano radiosketch01.ino
status=$( echo $? )
if [ "$status" = "1" ]; then
    echo Compilation unsuccessful, aborting.
    exit 1
    else
    echo Compilation successful, uploading.
    arduino-cli upload -p /dev/ttyUSB0 --fqbn arduino:avr:nano radiosketch01.ino
fi
exit 0