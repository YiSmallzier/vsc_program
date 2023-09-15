from gpiozero import LED
from time import sleep

def light(led):
    led.on()
    # sleep(1)
    # led.off()
    # sleep(1)
    # led.on()
    # sleep(1)
    # led.off()
    # sleep(1)

def call_led(power,mode):

    global red
    global green
    global blue
    print('in led')
    red.off()
    green.off()
    blue.off()

    if power :
        green.on()
        sleep(1)
        if mode == 2:
            green.off()
            sleep(1)
            blue.on()
            sleep(1)
    else:

        red.on()
        sleep(1)
    print('out led')
    



red.on()
sleep(1)
red.off()
# light(red)
# light(green)
# light(blue)




