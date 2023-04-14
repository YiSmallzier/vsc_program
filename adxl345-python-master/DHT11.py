import Adafruit_DHT, time
from gpiozero import MCP3008
from time import sleep

def convert_temp(gen):
    for value in gen:
        yield(value * 3.3 - 0.5)*100

adc = MCP3008(channel=0)

for temp in convert_temp(adc.values):
    print('The temperature is', temp, 'oC')
    sleep(1)
sensor = Adafruit_DHT.DHT11

gpio = 18

while True:
    hum, tem = Adafruit_DHT.read_retry(sensor, gpio)
    if hum is not None and tem is not None:
        print('Temp={0:0.1f}*C Humi={1:0.1f}%'.format(tem, hum))
    else:
        print('Failed')
    time.sleep(1)