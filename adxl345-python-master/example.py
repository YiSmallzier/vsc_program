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
adxl345 = ADXL345()
    
axes = adxl345.getAxes(True)
# print ("ADXL345 on address 0x%x:" % (adxl345.address))
# print ("   x = %.3fG" % ( axes['x'] ))
# print ("   y = %.3fG" % ( axes['y'] ))
# print ("   z = %.3fG" % ( axes['z'] ))
acc_x = axes['x']
acc_y = axes['y']
acc_z = axes['z']
# 报警标志位
alert_flag = False
# angle_pitch 为俯仰角，人体绕x轴旋转角度
# angle_roll 人左右倾斜角度， y轴旋转角度
# angle_yaw 人左右旋转，z轴旋转角度
angle_roll = math.atan(acc_x / math.sqrt(acc_y + acc_z))
angle_pitch = math.atan(acc_y / math.sqrt(acc_x + acc_z))
angle_yaw = math.atan(math.sqrt(acc_x + acc_y) / acc_z)

# 检测函数入口
def main_check():
    # 持续检测
    while True:
        if (loss_check):
            # 失重中断检测返回True，进入撞击中断检测
            if(hit_check):
                print("Alert!")
        sleep(0.01)


# 根据三个姿态角的临界状态检测阈值
# 前后和侧向跌倒时， angle_roll angle_pitch  发生很大变化， angle_yaw没有明显变化
acc = math.sqrt(acc_x + acc_y + acc_z)
eps = 1e-3
# 1.失重中断检测 , acc < 0.6 G 持续一定时间， 设为0.2s
def loss_check():
    for t in range(100):
        if(acc > 0.6):
            return False
        sleep(0.002)
    return True

# 2.撞击中断检测， 失重0.5s内， acc > 2 G (波峰) ， 设置采集100个采样点
def hit_check():
    for t in range(100):

        if acc > 2:

            for i in range(100):
                angle_roll = math.atan(acc_x / math.sqrt(acc_y + acc_z))
                angle_pitch = math.atan(acc_y / math.sqrt(acc_x + acc_z))
                angle_yaw = math.atan(math.sqrt(acc_x + acc_y) / acc_z)
                if(angle_pitch > 45 | angle_roll > 45):
                    alert_flag = True
                sleep(0.01)
            # 静止中断检测
            if (stop_check() and angle_check()):
                return True
            else:
                return False
        sleep(0.005)


# 3.静止中断检测， acc == 1 G >=2s
def stop_check():
    for i in range(100):
        if acc >= 1 + eps or acc <= 1 - eps:
            return False
    return True
# 4.姿态角检测， angle_pitch > 45 or angle_roll > 45 报警
def angle_check():
    return alert_flag







