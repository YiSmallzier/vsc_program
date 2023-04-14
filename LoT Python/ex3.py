from gpiozero import LineSensor
import platform
import time
from signal import pause
from time import sleep
cnt = 0
sumt = 0
last = 0

def line_detected ():
    print("Line detected")
    T1 = time.clock()
    global last
    global sumt,cnt
    sumt += T1 - last
    cnt += 1

    
    print("%.2f round/s"%float(cnt/sumt) )
 
    last = T1

def line_nodetected ():
    T2 = time.clock()
    print('No line detected')
    print(T2)

T1 = T2 = time.clock()
sensor = LineSensor(18)
while True:

    if abs(sensor.value) < 0.1:

        line_detected()
        sleep(0.5)
