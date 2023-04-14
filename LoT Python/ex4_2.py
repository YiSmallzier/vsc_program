# ADXL345 Python example 
#
# author:  Jonathan Williamson
# license: BSD, see LICENSE.txt included in this package
# 
# This is an example to show you how to use our ADXL345 Python library
# http://shop.pimoroni.com/products/adafruit-triple-axis-accelerometer

from adxl345 import ADXL345
import math
from time import sleep
from gpiozero import LED
from signal import pause
def loss_check():
    print('loss_check')
    for t in range(100):
        if(acc > 0.9):
            return False
        sleep(0.002)
    return True

def hit_check():
    print('hit check')
    global alert_flag
    for t in range(100):
        flash()
        print(acc)
        if acc > 1.4:

            for i in range(100):
                flash()
                print(angle_pitch,angle_roll)
                if(angle_pitch > 45 or angle_roll > 45):
                    alert_flag = True
                sleep(0.01)
           
            if (stop_check() and angle_check()):
                return True
            else:
                return False
        sleep(0.005)



def stop_check():
    print('stop check')

    for i in range(100):
        flash()
        if acc >= 1.2 or acc <= 0.8:
            return False
    return True

def angle_check():
    print('angle check')
    return alert_flag


adxl345 = ADXL345()

def flash():
    global axes, acc, acc_x, acc_z, acc_y, angle_roll , angle_pitch, angle_yaw
    axes = adxl345.getAxes(True)
    acc_x = abs(axes['x'])
    acc_y = abs(axes['y'])
    acc_z = abs(axes['z'])
    eps = 1e-8
    angle_roll = math.atan(acc_x / (math.sqrt(abs(acc_y) + abs(acc_z))+eps)) * 180 / math.pi
    angle_pitch = math.atan(acc_y / (math.sqrt(abs(acc_x) + abs(acc_z)) + eps))* 180 / math.pi
    angle_yaw = math.atan(math.sqrt(abs(acc_x) + abs(acc_y)) / (acc_z+eps))* 180 / math.pi
    acc = math.sqrt(acc_x + acc_y + acc_z)

leds = LED(18)
axes = adxl345.getAxes(True)
acc_x = abs(axes['x'])
acc_y = abs(axes['y'])
acc_z = abs(axes['z'])
# print(acc_x,acc_y,acc_z)
alert_flag = False
eps = 1e-8
angle_roll = math.atan(acc_x / (math.sqrt(abs(acc_y) + abs(acc_z))+eps)) * 180 / math.pi
angle_pitch = math.atan(acc_y / (math.sqrt(abs(acc_x) + abs(acc_z)) + eps))* 180 / math.pi
angle_yaw = math.atan(math.sqrt(abs(acc_x) + abs(acc_y)) / (acc_z+eps))* 180 / math.pi

acc = math.sqrt(acc_x + acc_y + acc_z)


while(True):
    flash()
    global alert_flag
    alert_flag = False
# def main_check():

    # while True:
    print(angle_roll,angle_pitch,angle_yaw)
    print(acc)
    # print(acc_x,acc_y,acc_z)
    if (loss_check()):
        
        if(hit_check()):
            leds.blink(0.2)
            # sleep(0.5)
            print("Alert!")
            pause()
    print(alert_flag)
    sleep(0.2)







# main_check()

