import os
import time

class adb_controller:
    def __init__(self) -> None:
        pass
    def open_app():
        os.system("adb shell am start -n com.duokan.phone.remotecontroller/com.xiaomi.mitv.phone.remotecontroller.HoriWidgetMainActivityV2")
        time.sleep(2)

    def open_controller():
        os.system("adb shell am start -n com.duokan.phone.remotecontroller/com.xiaomi.mitv.phone.remotecontroller.HoriWidgetMainActivityV2")
        time.sleep(5)
        os.system("adb shell input tap 100 300")
        time.sleep(2)
    
    def click_power():
        os.system("adb shell input tap 100 700")
        time.sleep(2)

    def click_mode():
        os.system("adb shell input tap 400 700")
        time.sleep(2)

    def go_home():
        os.system("adb shell input keyevent 3")
        time.sleep(2)