# tmp, hum monitor 
# readme:
# this file use dead loop (while True) to monitor tmp and hum forever
# how to use: 
# in another file: os.system("python3 %s %s" % current, powerof ) 

import gpiozero
import Adafruit_DHT
tmp_sensor = Adafruit_DHT.DHT11
import sys

import time
from get_module import get_tmp

MAX_TEM = 26
MAX_HUM = 90

def get_monitor(current_power, current_mode):
    MAX_TEM = 26.
    MAX_HUM = 90.
    # current_power shifted state of power
    # 0: power_off 
    # 1: power_on
    # current_mode
    # 0: power_off 
    # 1: cold 
    # 2: dehum

    hum, tem = get_tmp()
    #tem > 26C, mode -> cold
    if tem > MAX_TEM:
        print(f"MAX_LIMIT_TEM1{MAX_TEM}+{tem}")
        power_flag = current_power
        #dehum -> cold
        mode_flag = 1
    # tem < 26C and HUM > 90
    elif hum > MAX_HUM:
        print("MAX_LIMIT_HUM2")
        power_flag = current_power
        #cold -> dehum
        mode_flag = 2
    else:
        power_flag = 0
        mode_flag = 0

    return power_flag, mode_flag

def test_get_monitor(hum,tem):
    MAX_TEM = 26.
    MAX_HUM = 90.
    # current_power shifted state of power
    # 0: power_off 
    # 1: power_on
    # current_mode
    # 0: power_off 
    # 1: cold 
    # 2: dehum

    # hum, tem = 
    #tem > 26C, mode -> cold
    if tem > MAX_TEM:
        print(f"MAX_LIMIT_TEM1{MAX_TEM}+{tem}")
        power_flag = 1
        #dehum -> cold
        mode_flag = 1
    # tem < 26C and HUM > 90
    elif hum > MAX_HUM:
        print("MAX_LIMIT_HUM2")
        power_flag = 1
        #cold -> dehum
        mode_flag = 2
    else:
        power_flag = 0
        mode_flag = 0

    return power_flag, mode_flag


def monitor(current_power, current_mode):
    while True:
        #!!! make sure power_off
        mode = 0
        tem, hum = get_tmp()
        # step: time to sleep in once loop
        step = 3 # 60
        # num: numbers of loop
        num = 5 # 10

        #tem > 26C, mode -> cold
        if tem > MAX_TEM:
            if current_power == 0:
                power_flag = 1
            elif current_mode == 2:
                #dehum -> cold
                mode_flag = 1
        # tem < 26C and HUM > 
        elif hum > MAX_HUM:
            if current_power == 0:
                power_flag = 1
            elif current_mode == 1:
                #cold -> dehum
                mode_flag = 2
        
        t = t + step
        if t > step*num:
            #shut down exe, make sure mode is cold
            if mode == 1:
                print("Mode rollback")
                # dehum_to_cold()
            exit()
        # test situation:5s, usually: at least 60s   
        time.sleep(step)

if __name__ == "__main__":
    monitor(0, 0)
