from gpiozero import Motor
from gpiozero import PhaseEnableRobot
from gpiozero import OutputDevice as stepper
from time import sleep 
import sys


# motor_pin1 = stepper(pin=2,initial_value=True)
# motor_pin1.value = 0.5
# motor_pin2 = stepper(pin=3,initial_value=True)
# motor_pin2.value = 0
# # motor_pin3 = stepper(5)
# # motor_pin4 = stepper(6)
# # main#initial_value = OutputDevice(True)
# while(True):
#     sleep(1)
#     motor_pin1.off()
#     motor_pin2.off()
#     sleep(1)
#     motor_pin1.on()
#     motor_pin2.on()
#     motor_pin1.value = 0.5
#     motor_pin2.value = 0


from gpiozero import Motor
from gpiozero import PhaseEnableRobot
from gpiozero import OutputDevice as stepper
from time import sleep 
import sys



IN1 = 2
IN2 = 3
IN3 = 5
IN4 = 6


# seq = [[1,0,0,0],
#        [1,1,0,0],
#        [0,1,0,0],
#        [0,1,1,0],
#        [0,0,1,0],
#        [0,0,1,1],
#        [0,0,0,1],
#        [1,0,0,1]]

seq = [[1,0,0,0],
        [0,1,0,0],
        [0,0,1,0],
        [0,0,0,1]]

def rotate(steps, direction):
    for i in range(steps):
        if direction == 1:
            print('+')
            for step in seq:
                Bd.value = step[0]
                Ad.value = step[1]
                Ba.value = step[2]
                Aa.value = step[3]
                sleep(0.01)
        elif direction == -1:
            print('-')
            for step in reversed(seq):
                # Aa.off()
                Bd.value = step[0]
                Ad.value = step[1]
                Ba.value = step[2]
                Aa.value = step[3]
                sleep(0.01)
        else:
            break




Aa = stepper(IN1)
Ad = stepper(IN2)
Ba = stepper(IN3)
Bd = stepper(IN4)

angle = 0

step_dir = -1
print('111')

while True:
    step_dir = -step_dir

    rotate(1, step_dir)
    angle = 45*step_dir
    print(f'angle: {angle} degrees')
    sleep(1)

    rotate(2, step_dir) 
    angle = 90*step_dir
    print(f"angle: {angle} degrees")
    sleep(1)

    rotate(4, -step_dir) 
    angle = 180*step_dir
    print(f"angle: {angle} degrees")
    sleep(1)

    rotate(8, step_dir)
    angle = 360*step_dir
    print(f"angle: {angle} degrees")
    sleep(1)
