import numpy as np
import matplotlib.pyplot as plt
from gpiozero import AngularServo, DistanceSensor
import time
import math

from time import sleep


sensor = DistanceSensor(25, 8, max_distance = 2.0)

# 设置底部舵机和云台舵机
pan_servo = AngularServo(18)
tilt_servo = AngularServo(17)

# 设置扫描范围
pan_min_angle = 0
pan_max_angle = 180
tilt_min_angle = 0
tilt_max_angle = 90

# 设置扫描步长和测距间隔时间
pan_step = 2
tilt_step = 2
delay = 0.1

# 设置图像参数
r_max = 2.0
theta_max = 2 * math.pi
theta = np.linspace(0, theta_max, 360)
r = np.zeros(360)

# 创建 Matplotlib 的极坐标图形
fig = plt.figure(figsize=(8, 8))
ax = fig.add_subplot(111, projection='polar')

# 循环扫描
for tilt_angle in range(tilt_min_angle, tilt_max_angle+tilt_step, tilt_step):
    tilt_servo.angle = tilt_angle

    for pan_angle in range(pan_min_angle, pan_max_angle+pan_step, pan_step):
        pan_servo.angle = pan_angle
        time.sleep(delay)

        # 测量距离
        distance = sensor.distance
        print(distance)

        # 距离超出范围则跳过
        if distance > r_max:
            continue

        # 计算极坐标下的点位置
        index = int(pan_angle / pan_max_angle * 360)
        r[index] = distance

    # 绘制雷达图
    ax.clear()
    ax.set_ylim([0, r_max])
    ax.plot(theta, r)
    fig.canvas.draw()


# 舵机测试代码

# servo = AngularServo(18, min_angle = 30, max_angle = 150)

# while True:
#     servo.angle = 30
#     sleep (2)
#     servo.angle = 90
#     sleep (2)
#     servo.angle = 150
#     sleep (2)
#     servo.angle = 90

