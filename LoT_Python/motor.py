from gpiozero import Motor
from gpiozero import PhaseEnableRobot
from gpiozero import OutputDevice as stepper
from gpiozero import PWMOutputDevice
from time import sleep 
import sys


# # Test 1： 使用OutputDevice类驱动直流电机
motor_pin = stepper(2, inittial_value = False)
motor_pin.on()
# main

while True:
    motor_pin.forward(0.5)
    
    sleep(5)
    motor_pin.off()





# # Test2 : 步进电机
IN1 = 2
IN2 = 3
IN3 = 8
IN4 = 9
# 步进电机 GPIO: [2, 3, 8, 9]

seq = [[1,0,0,0],
       [0,1,0,0],
       [0,0,1,0],
       [0,0,0,1]]

seq_back = [[1,0,0,0],
            [0,0,0,1],
            [0,0,1,0],
            [0,1,0,0]]

def rotate(steps, direction):
    for i in range(steps):
        if direction == 1:
            for step in seq:
                Aa.value = step[0]
                Ad.value = step[1]
                Ba.value = step[2]
                Bd.value = step[3]
                sleep(0.01)
        elif direction == -1:
            for step in seq_back:
                Aa.value = step[0]
                Ad.value = step[1]
                Ba.value = step[2]
                Bd.value = step[3]
                sleep(0.01)
        else:
            break

def main():
    
    # 初始化电机引脚
    Aa = stepper(IN1)
    Ad = stepper(IN2)
    Ba = stepper(IN3)
    Bd = stepper(IN4)

    angle = 0 # 初始旋转角度为0

    step_dir = -1
    # step_dir = -1 时反转， step_dir = 1 时正转

    while True:
        step_dir = -step_dir

        rotate(8, step_dir) # 转45度
        angle = 45
        print(f"angle: {angle} degrees")
        sleep(1)

        rotate(16, step_dir) # 转90度
        angle = 90
        print(f"angle: {angle} degrees")
        sleep(1)

        rotate(32, step_dir) # 转180度
        angle = 180
        print(f"angle: {angle} degrees")
        sleep(1)

        rotate(64, step_dir) # 转360度
        angle = 360
        print(f"angle: {angle} degrees")
        sleep(1)

    









