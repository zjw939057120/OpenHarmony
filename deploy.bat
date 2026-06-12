@echo off

adb root
adb push .\build\spidev_test /data
adb shell chmod 777 /data/spidev_test
adb shell /data/spidev_test
