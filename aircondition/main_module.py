from asyncio import sleep
from socket import timeout
import gpiozero
from gpiozero import LED
import Adafruit_DHT

tmp_sensor = Adafruit_DHT.DHT11

from control_module import branch, after_live, test_branch, ControlAttr

import configparser

import time
import os
from adb_module import adb_controller
# import LED_module

attr = ControlAttr(
    root_ctrl = False,
    power_flag = False,
    mode_flag = 1,
    shift_power = False,
    shift_mode = 1
)
temp = 24
hum = 84
red = LED(23)
green = LED(24)
blue = LED(25)
def run_main():
    global attr
    print("Welcome to use Smart AirCondition Controller!")
    if attr.power_flag:
        green.on()
        # sleep(1)
        if attr.mode_flag == 2:
            green.off()
            # sleep(1)
            blue.on()
            # sleep(1)
    else:
        red.on()
    adb_controller.open_app()
    adb_controller.open_controller()
    for i in range(3):
        # attr = live(attr)
        attr = test_live(attr)
    if attr.mode_flag == 2:
        adb_controller.click_mode()
    if attr.power_flag:
        adb_controller.click_power()
    # adb_controller.go_home()
    red.off()
    green.off()
    blue.off()

def test_live(attr):
    global temp
    global hum
    for i in range(5):
        print(i)
        attr = test_branch(attr,2,('11','35'),79,temp+i)
        time.sleep(1)
    temp = temp + 5
    # hum = hum + 5
    attr = after_live(attr)
    print('power_flag'+str(attr.power_flag))
    print('mode_flag'+str(attr.mode_flag))
    
    if attr.power_flag:
        green.on()
        # sleep(1)
        if attr.mode_flag == 2:
            green.off()
            # sleep(1)
            blue.on()
            # sleep(1)
    else:
        red.on()

    return attr



def live(attr):
    for i in range(5):
        print(i)
        attr = branch(attr)
        time.sleep(1)
    attr = after_live(attr)
    if attr.power_flag:
        green.on()
        # sleep(1)
        if mode_flag == 2:
            green.off()
            # sleep(1)
            blue.on()
            # sleep(1)
    else:
        red.on()
        # sleep(1)
    # LED_module.call_led(power_flag, mode_flag)
    return attr

# gpio = TMP_GPIO


# more...


def load_config():
    cf = configparser.ConfigParser()
    cf.read("config.ini")
    secs = cf.sections()

    print(secs)

    options = cf.options("default")
    print(options)

    MAX_TMP = cf.get("default", "MAX_TMP")
    MIN_TMP = cf.get("default", "MIN_TMP")
    MAX_HUM = cf.get("default", "MAX_HUM")
    print(MAX_TMP, MIN_TMP, MAX_HUM)


    return

import numpy as np
import copy

def write_config(val):
    cf = configparser.ConfigParser()
    filename = "config.ini"
    cf.read(filename)
    node = "default"
    key = "MAX_TMP"
    value = "val"
    cf.set(node, key, value)
    # fh = open(filename, "w")
    # cf.write(fh)
    with open(filename, 'w') as configfile:
        cf.write(configfile)
    # fh.close()
    return 

from get_module import get_tmp


def tmp_test():
    hum, tem = get_tmp()
    print(hum, tem)
    
    return 
    
def adb_test():
    #
    print("app test")
    os.system("adb shell am start -n com.duokan.phone.remotecontroller/com.xiaomi.mitv.phone.remotecontroller.HoriWidgetMainActivityV2")
    time.sleep(5)

    #
    print("controller test")
    os.system("adb shell input tap 100 300")
    time.sleep(1)
    
    #
    print("power test")
    os.system("adb shell input tap 100 700")
    time.sleep(1)
    
    #
    print("mode test")
    os.system("adb shell input tap 400 700")
    time.sleep(1)

    #
    print("home test")
    os.system("adb shell input keyevent 3")

    ###
    print("All test finished")
    return 

def test_main():
    # ...
    print("Welcome to use Smart AirCondition Controller!")
    # print("use config.ini ? (Y/n)")
    # op = input()
    # if op.lower() == 'y':
    #     load_config()

    # print("Test module: Tmp & Hum:")
    # tmp, hum = get_tmp()


    # print("Test module Key:")
    # print("send power")
    # send_power()

    # time.sleep(1)
    # print("send mode")
    # send_mode()

    # time.sleep(1)
    # print("send add")
    # # send_add()

    # time.sleep(1)
    # print("send dec")
    # # send_dec()

    # print("Test module Person Detect:")
    # #...

    # print("All moudles tested successful!")
    # #...
    # return

    print("adb_module test:")
    adb_test()
    print("get_module test:")
    tmp_test()




# if __name__ == "__main__":
# test_main()
run_main()






