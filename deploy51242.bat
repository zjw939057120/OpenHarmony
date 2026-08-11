@echo off

adb root
@REM 引脚	GPIO引脚	GPIO引脚号
@REM AI5_A	SGM4591XTS24G	284
@REM AI5_B	SGM4591XTS24G	285
@REM AI5_C	SGM4591XTS24G	286
@REM AI6_A	SGM4591XTS24G	287
@REM AI6_B	SGM4591XTS24G	256
@REM AI6_C	SGM4591XTS24G	257
@REM AI7_A	SGM4591XTS24G	258
@REM AI7_B	SGM4591XTS24G	259
@REM AI7_C	SGM4591XTS24G	260
@REM AI8_A	SGM4591XTS24G	261
@REM AI8_B	SGM4591XTS24G	262
@REM AI8_C	SGM4591XTS24G	263
@REM AI9_A	SGM4591XTS24G	264
@REM AI9_B	SGM4591XTS24G	265
@REM AI9_C	SGM4591XTS24G	266
@REM AI10_A	SGM4591XTS24G	267
@REM AI10_B	SGM4591XTS24G	268
@REM AI10_C	SGM4591XTS24G	269
@REM AI11_A	SGM4591XTS24G	270
@REM AI11_B	SGM4591XTS24G	271
@REM AI11_C	SGM4591XTS24G	52
@REM adb shell "echo 261 > /sys/class/gpio/export"
@REM adb shell "echo 262 > /sys/class/gpio/export"
@REM adb shell "echo 263 > /sys/class/gpio/export"

@REM adb shell "echo out > /sys/class/gpio/gpio261/direction"
@REM adb shell "echo out > /sys/class/gpio/gpio262/direction"
@REM adb shell "echo out > /sys/class/gpio/gpio263/direction"

@REM adb shell "echo 1 > /sys/class/gpio/gpio261/value"
@REM adb shell "echo 1 > /sys/class/gpio/gpio262/value"
@REM adb shell "echo 0 > /sys/class/gpio/gpio263/value"


adb shell rm /data/sgm51242
adb push .\build\sgm51242 /data
adb shell chmod 777 /data/sgm51242
adb shell /data/sgm51242
