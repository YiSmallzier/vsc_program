from gpiozero import TrafficLights,LED , DistanceSensor
from gpiozero import Button
from time import sleep
from signal import pause
from multiprocessing import Process

leds = LED(2)
sensor = DistanceSensor(25,8,max_distance = 0.2,threshold_distance=0.1)

while(1):
    if(sensor.distance <= 0.05):
        print('now distance:' + str(sensor.distance*100))
        print('percent error: ' + 100*abs(5 - sensor.distance*100)/5 + '%') 
        leds.off()
        leds.blink(0.02)
        sleep(0.2)
        print('distance out')
    else:
        print('now distance:' + str(sensor.distance*100))
        leds.off()
        leds.on()
        sleep(0.2)
        print('distance in')