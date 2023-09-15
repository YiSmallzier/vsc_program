import lirc

def pasreset(data): #解析按键
    if data == 'echo "KEY_1"':
        print("1 按下") #遥控器按下1:
    elif data == 'echo "KEY_2"':
        print("2 按下") #遥控器按下2:
    elif data == 'echo "KEY_3"':
        print("3 按下") #遥控器按下3:

if __name__ == "__main__":

    with lirc.LircdConnection("test.py",) as conn:
        while True:
            string = conn.readline()
            pasreset(string)
            #print("收到:",end = '')
            #print(type(string))
