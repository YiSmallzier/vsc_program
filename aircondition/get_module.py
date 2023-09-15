import gpiozero
import Adafruit_DHT
import time
tmp_sensor = Adafruit_DHT.DHT11

# GPIO DHT11
gpio = 18

def convert_tmp():
    timeout_clk = 0
    while True:
        timeout_clk = timeout_clk + 1
        if timeout_clk >= 10:
            break
        hum, tem = Adafruit_DHT.read_retry(tmp_sensor, gpio)
        if hum is not None and tem is not None:
            print('Temp = {0:0.1f}*C Humi = {1:0.1f}'.foramt(tem, hum))
        else:
            print('Tmp failed')
        
        time.sleep(1)
    return 


def get_tmp():
    hum, tem = Adafruit_DHT.read_retry(tmp_sensor, gpio)
    if hum is not None and tem is not None:
        print('Temp = {0:0.1f}*C Humi = {1:0.1f}%'.format(tem, hum))
        return hum, tem
    else:
        print('Tmp failed')
        
    return (None, None)
    

def get_test():
    print("import get_module")
