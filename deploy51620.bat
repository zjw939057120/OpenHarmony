@echo off

adb root
@REM 引脚	GPIO引脚	GPIO引脚号
@REM AI1_A	SGM4591XTS24G	272
@REM AI1_B	SGM4591XTS24G	273
@REM AI1_C	SGM4591XTS24G	274
@REM AI2_A	SGM4591XTS24G	275
@REM AI2_B	SGM4591XTS24G	276
@REM AI2_C	SGM4591XTS24G	277
@REM AI3_A	SGM4591XTS24G	278
@REM AI3_B	SGM4591XTS24G	279
@REM AI3_C	SGM4591XTS24G	280
@REM AI4_A	SGM4591XTS24G	281
@REM AI4_B	SGM4591XTS24G	282
@REM AI4_C	SGM4591XTS24G	283
adb shell "echo 281 > /sys/class/gpio/export"
adb shell "echo 282 > /sys/class/gpio/export"
adb shell "echo 283 > /sys/class/gpio/export"

adb shell "echo out > /sys/class/gpio/gpio281/direction"
adb shell "echo out > /sys/class/gpio/gpio282/direction"
adb shell "echo out > /sys/class/gpio/gpio283/direction"

adb shell "echo 0 > /sys/class/gpio/gpio281/value"
adb shell "echo 1 > /sys/class/gpio/gpio282/value"
adb shell "echo 1 > /sys/class/gpio/gpio283/value"


adb shell rm /data/sgm51620
adb push .\build\sgm51620 /data
adb shell chmod 777 /data/sgm51620
adb shell /data/sgm51620
