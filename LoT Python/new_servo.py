import numpy as np
import pyqtgraph as pg
import time
from gpiozero import DistanceSensor, AngularServo

# 初始化GPIO口和舵机
servo_horizontal = AngularServo(18, min_angle=0, max_angle=180)
servo_vertical = AngularServo(17, min_angle=0, max_angle=180)
sensor = DistanceSensor(25, 8, max_distance = 0.2)

# 创建绘图应用
app = pg.QtGui.QApplication([])
win = pg.GraphicsWindow(title="Radar")
win.resize(800, 800)
view = win.addPlot()
view.setAspectLocked(True)

# 创建绘图项
scatter = pg.ScatterPlotItem(size=10, brush=pg.mkBrush(255, 255, 255, 120))
view.addItem(scatter)

# 初始化绘图数据
angle = 0
data = np.array([[angle, sensor.distance * np.cos(np.deg2rad(angle))]])  # 极坐标系下的坐标值

# 定义更新绘图函数
def update():
    global angle, data
    
    # 更新舵机角度
    servo_horizontal.angle = angle
    time.sleep(0.02)
    
    # 计算新的坐标值
    angle += 1
    if angle >= 360:
        angle = 0
    new_data = np.array([[angle, sensor.distance * np.cos(np.deg2rad(angle))]])
    data = np.concatenate([data, new_data])
    
    # 更新绘图
    scatter.setData(x=data[:, 1] * np.sin(np.deg2rad(data[:, 0])), y=data[:, 1] * np.cos(np.deg2rad(data[:, 0])))

# 启动定时器，每隔500毫秒更新一次绘图
timer = pg.QtCore.QTimer()
timer.timeout.connect(update)
timer.start(20)

# 运行绘图应用
pg.QtGui.QApplication.instance().exec_()
