import Adafruit_DHT,time
sensor = Adafruit_DHT.DHT11

gpio = 19
ax = []
ay1 = []
ay2 = []
# plt.ion() 
ind = 0
# matplotlib.use('svg')
# plt.figure()
while True:
    hum, tem = Adafruit_DHT.read_retry(sensor,gpio)
    if hum is not None and tem is not None:
        print('Temp={0:0.1f}*C Humi={1:0.1f}%'.format(tem,hum))
    else:
        print('Failed ')
    ax.append(ind)
    ind += 1
    ay1.append(tem)
    ay2.append(hum)
    if(ind == 60):
        print(ax)
        print(ay1)
        print(ay2)
    time.sleep(1)