from PIL import ImageGrab
import numpy as np
import cv2
from glob import glob
import numpy as np
import time
import os

#Python将数字转换成大写字母
def getChar(number):
    factor, moder = divmod(number, 26) # 26 字母个数
    modChar = chr(moder + 65)          # 65 -> 'A'
    if factor != 0:
        modChar = getChar(factor-1) + modChar # factor - 1 : 商为有效值时起始数为 1 而余数是 0
    return modChar
def getChars(length):
    return [getChar(index) for index in range(length)]


def Heise_zhanbi(img):
    [height, width, tongdao] = img.shape

    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

    a = np.sum(gray < 125)
    zhanbi = (float)(a) / (float)(height*width)

    return zhanbi

def Baise_zhanbi(img):
    [height, width, tongdao] = img.shape

    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)


    b=np.sum(gray>235)
    zhanbi = (float)(b) / (float)(height*width)

    return zhanbi


def dingweiqizi_weizhi(img):
    '''********************************************
    1、定位棋盘位置
    ********************************************'''

    image = img.copy()
    w, h, c = img.shape
    img2 = np.zeros((w, h, c), np.uint8)
    img3 = np.zeros((w, h, c), np.uint8)

    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    lower = np.array([10, 0, 0])
    upper = np.array([40, 255, 255])
    mask = cv2.inRange(hsv, lower, upper)
    erodeim = cv2.erode(mask, None, iterations=2)  # 腐蚀
    dilateim = cv2.dilate(erodeim, None, iterations=2)

    img = cv2.bitwise_and(img, img, mask=dilateim)
    frame = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    ret, dst = cv2.threshold(frame, 100, 255, cv2.THRESH_BINARY)
    contours, hierarchy = cv2.findContours(dst, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)


    i = 0
    maxarea = 0
    nextarea = 0
    maxint = 0
    for c in contours:
        if cv2.contourArea(c) > maxarea:
            maxarea = cv2.contourArea(c)
            maxint = i
        i += 1

    # 多边形拟合
    epsilon = 0.02 * cv2.arcLength(contours[maxint], True)
    if epsilon < 1:
        print("error :   epsilon < 1")
        pass

    # 多边形拟合
    approx = cv2.approxPolyDP(contours[maxint], epsilon, True)
    [[x1, y1]] = approx[0]
    [[x2, y2]] = approx[2]

    checkerboard = image[y1:y2, x1:x2]


    return checkerboard


def dingweiqizi_yanse_weizhi(img):
    '''********************************************
    2、识别棋盘棋子位置及颜色及序号；
    ********************************************'''

    img = cv2.resize(img, (724,724), interpolation=cv2.INTER_AREA)

    
    #变量定义
    small_length=38  #每个小格宽高
    qizi_zhijing=38 #棋子直径

    list = [[0 for i in range(19)] for j in range(19)]
    #print(list)

    for i in range(19):
        for j in range(19):

            lie = i
            hang = j

            Tp_x = small_length * lie
            Tp_y = small_length * hang
            Tp_width = qizi_zhijing
            Tp_height = qizi_zhijing

            img_temp=img[Tp_y:Tp_y+Tp_height, Tp_x:Tp_x+Tp_width]#参数含义分别是：y、y+h、x、x+w

            heise_zhanbi=Heise_zhanbi(img_temp)
            if heise_zhanbi>0.5:
                list[hang][lie]=2#黑色
                print("[",j+1,",",i+1,"]: 黑棋")
            else:
                baise_zhanbi = Baise_zhanbi(img_temp)
                if baise_zhanbi > 0.15:
                    list[hang][lie] = 1  # 白色
                    print("[",j+1,",",i+1,"]: 白棋")
                else:
                    list[hang][lie] = 0  # 无棋子

    # 画出垂直方向的线
    for i in range(1, 19):
        cv2.line(img, (i * 38, 19), (i * 38, 705), (255, 0, 0), 2)

    # 画出水平方向的线
    for j in range(1, 19):
        cv2.line(img, (19, j * 38), (705, j * 38), (255, 0, 0), 2)
    cv2.imshow("1", img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()
    cv2.imwrite('res.jpg', img)

    return  list


def check_win(board):
    # 检查行
    for i in range(19):
        for j in range(15):
            if board[i][j] == board[i][j+1] == board[i][j+2] == board[i][j+3] == board[i][j+4] != 0:
                return board[i][j]
    
    # 检查列
    for i in range(15):
        for j in range(19):
            if board[i][j] == board[i+1][j] == board[i+2][j] == board[i+3][j] == board[i+4][j] != 0:
                return board[i][j]
    
    # 检查对角线1
    for i in range(15):
        for j in range(15):
            if board[i][j] == board[i+1][j+1] == board[i+2][j+2] == board[i+3][j+3] == board[i+4][j+4] != 0:
                return board[i][j]
    
    # 检查对角线2
    for i in range(15):
        for j in range(4, 19):
            if board[i][j] == board[i+1][j-1] == board[i+2][j-2] == board[i+3][j-3] == board[i+4][j-4] != 0:
                return board[i][j]

    return 0


if __name__ =="__main__":
    list0 = [[0 for i in range(19)] for j in range(19)]
    list_finall = []
    connter = 0

    img = cv2.imread("./screen/1.png")
    '''********************************************
    1、定位棋盘位置
    ********************************************'''
    img_after=dingweiqizi_weizhi(img)
    
    '''********************************************
    2、识别棋盘棋子位置及颜色及序号；
    ********************************************'''
    list1=dingweiqizi_yanse_weizhi(img_after)

    if check_win(list1) != 0:
        print('One player win!')
    
    for i in range(19):
        print(list1[i])
    

    # 模拟五子连排的情况
    print("DEBUG:")
    for i in range(2, 7):
        list1[i][0] = 1
    for i in range(19):
        print(list1[i])
    if check_win(list1) != 0:
        print('One player win!')
