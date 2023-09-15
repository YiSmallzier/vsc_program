import os
import sys
import time
import person_module
import timer_module
from adb_module import adb_controller
import monitor


# root_ctrl = False
# power_flag = False
# mode_flag = 1

# shift_power = False
# shift_mode = 1
class ControlAttr:
    def __init__(self, root_ctrl=False, power_flag=False, mode_flag=1, shift_power=False, shift_mode=1):
        self.root_ctrl = root_ctrl
        self.power_flag = power_flag
        self.mode_flag = mode_flag
        self.shift_power = shift_power
        self.shift_mode = shift_mode

    # def update(self, attr):
    #     self.shift_power = attr.shift_power
    #     self.shift_mode = attr.shift_mode
    def update(self, sp, sm):
        self.shift_power = sp
        self.shift_mode = sm


def test_branch(attr, person_num, time, hum, tem):
    curr_power = True
    curr_mode = 0

    # if root has controled
    if attr.root_ctrl:
        attr.shift_power = False
        return attr
    print(f'hum:{hum},temp:{tem}')
    # detect person
    # curr_detect = person_module.person_detect()
    curr_detect = person_num
    if not curr_detect:
        # curr_time = timer_module.get_time()
        curr_time = time
        if curr_time is None:
            raise Exception("Failed to get current time")
        # 11.30 ~ 12.30 or 18.30 ~ 21.50
        _hour_min = curr_time[0] + curr_time[1]
        if ('1130' <= _hour_min <= '1230') or ('1830' <= _hour_min <= '2150'):
            curr_power = True
        else:
            curr_power = False

    # ture to open ac
    if curr_power:
        curr_power, curr_mode = monitor.test_get_monitor(hum, tem)

    attr.shift_power = attr.power_flag ^ curr_power
    attr.shift_mode = curr_mode
    return attr


def branch(attr):
    global root_ctrl
    global shift_power
    global shift_mode
    global power_flag
    global mode_flag

    curr_power = True
    curr_mode = 0

    # if root has controled
    if attr.root_ctrl:
        attr.shift_power = False
        return attr

    # detect person
    curr_detect = person_module.person_detect()
    if not curr_detect:
        curr_time = timer_module.get_time()
        if curr_time is None:
            raise Exception("Failed to get current time")
        # 11.30 ~ 12.30 or 18.30 ~ 21.50
        _hour_min = curr_time[0] + curr_time[1]
        if ('1130' <= _hour_min <= '1230') or ('1830' <= _hour_min <= '2150'):
            curr_power = True
        else:
            curr_power = False

    # ture to open ac
    if curr_power:
        curr_power, curr_mode = monitor.get_monitor(attr.power_flag, attr.mode_flag)

    attr.shift_power = attr.power_flag ^ curr_power
    attr.shift_mode = curr_mode
    return attr


# branch(1, (27, 70), (11, 47))
# print(power_flag, shift_power, )
# # sleep 60s
#
# branch(1, (27, 80), (11, 48))
# print(power_flag, shift_power, )
# # sleep 60s
#
# branch(0, (27, 90), (11, 49))
# print(power_flag, shift_power, )
# # sleep 60s
#
# branch(0, (27, 80), (11, 50))
# print(power_flag, shift_power, )

# root_ctrl = True
# branch(1, (27, 80), (11, 51))
# print(poweron, need_open, need_close)

def after_live(attr):
    print(f"444{attr.power_flag},{attr.shift_power}")

    attr.power_flag = attr.power_flag ^ attr.shift_power

    print(f"555{attr.power_flag},{attr.shift_power}")
    if attr.shift_power:
        adb_controller.click_power()
        if attr.power_flag:
            print("open")  ###
        else:
            print("close")  ###

    if attr.power_flag:
        if attr.shift_mode and attr.shift_mode != attr.mode_flag:
            adb_controller.click_mode()
            attr.mode_flag = attr.shift_mode
            print("mode change")
            if attr.shift_mode == 1:
                print("cold")  ###
            else:
                print("dehum")  ##
                adb_controller.click_mode()
                adb_controller.click_mode()
                adb_controller.click_mode()
    print(f"666{attr.power_flag},{attr.shift_power}")
    attr.shift_power = False
    attr.shift_mode = False
    return attr





