import serial
from svgpathtools import svg2paths, wsvg, Line, QuadraticBezier
from xml.dom import minidom
import struct
import time
import sys

lineType = type(Line(start=(0+0j),end=(1,1j)))
quadType = type(QuadraticBezier(start=(0+0j), control=(.5+.5j), end=(1+1j)))

poesvg = open('PoE.svg', 'r')

doc = minidom.parse(poesvg)  # parseString also exists
viewBox = [el.getAttribute('viewBox') for el
        in doc.getElementsByTagName('svg')]
doc.unlink()
# print(viewBox) # maps to 0 0 100 100 on our coordinates
cxn = serial.Serial('/dev/ttyUSB0', baudrate=9600)

def pointScale(viewBox, point):
    # This is bad but I'm under time pressure again
    minx = float(viewBox[0].split()[0])
    miny = float(viewBox[0].split()[1])
    maxx = float(viewBox[0].split()[2])
    maxy = float(viewBox[0].split()[3])
    xScale = 1000/(maxx-minx)
    yScale = 1000/(maxy-miny)
    scale = min(xScale, yScale)
    return(int(point.real *scale), int(point.imag*scale))

def send(number):
    cxn.write(struct.pack('i', number))

def recieve():
    haveRevieved = False
    i = 0
    recievedVal = None
    while haveRevieved == False:
        if(cxn.in_waiting>0):
            recievedVal = cxn.read(cxn.in_waiting)
            # result = ''.join([i for i in recievedVal if i.isdigit()])
            haveRevieved = True
        else:
            time.sleep(.01)
            i+=1
            if i > 100:
                return
    try:
        point = recievedVal.decode().split(" ")
        for i in range(len(point)):
            point[i] = int(point[i])
        # print(point)
        return point

    except:
        e = sys.exc_info()[0]
        print(e)
        print(recievedVal.decode())
        return None

    time.sleep(.1)
    # except:
    #     print(recievedVal.decode())


def sendPoint(point):
    send(point[0])
    time.sleep(.01)
    send(point[1])
    time.sleep(.1)

def confirmPoint(point, pointNumber):
    hasSentCorrectly = False
    while not(hasSentCorrectly):
        print("Python sends point " + str(pointNumber) + " ",point)
        sendPoint(point)
        time.sleep(.2)
        valRec = recieve()
        if valRec == None or tuple(valRec) != point:
            print("ERROR!!!")
        if tuple(valRec) == point:
            hasSentCorrectly = True
            print("Correct value has been confirmed")


time.sleep(1)
recieve()

paths, attributes = svg2paths('PoE.svg')
for path in paths:
    for seg in path:
        if(type(seg)==lineType):
            counter = 0
            send(2)
            print("sent 2")
            valRec = recieve()
            print(valRec[0])

            for i in range(0,2):
                pointToSend = pointScale(viewBox, seg.point(i))
                confirmPoint(pointToSend, i)


        if(type(seg)==quadType):
            counter = 0
            send(8)
            print("sent 8")
            valRec = recieve()
            print(valRec[0])
            for i in range(0,8):
                pointToSend = pointScale(viewBox, seg.point(i))
                confirmPoint(pointToSend, i)
        print('\n')

    time.sleep(1)

    print('\n')
