import serial
from svgpathtools import svg2paths, wsvg, Line, QuadraticBezier
from xml.dom import minidom
import struct
import time

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
    while haveRevieved == False:
        if(cxn.in_waiting>0):
            recievedVal = cxn.read(cxn.in_waiting)
            # result = ''.join([i for i in recievedVal if i.isdigit()])
            haveRevieved = True
        else:
            time.sleep(.01)
    print(recievedVal)
    time.sleep(.1)


def sendPoint(point):
    send(point[0])
    time.sleep(.01)
    send(point[1])
    time.sleep(.1)

def confirmPoint(counter, point):
    x=0
    while x<counter:
        while cxn.inWaiting():
            print(cxn.readline())
            x += 1

time.sleep(1)
recieve()

paths, attributes = svg2paths('PoE.svg')
for path in paths:
    for seg in path:
        if(type(seg)==lineType):
            counter = 0
            send(2)
            recieve()
            # confirmPoint(1,0)
            print("Python sends point 0 ", pointScale(viewBox, seg.point(0)))
            sendPoint(pointScale(viewBox, seg.point(0)))
            # confirmPoint(2, 0)
            # print(cxn.readline())
            # recieve()
            time.sleep(.2)
            recieve()

            print("Python sends point 1 ", pointScale(viewBox, seg.point(1)))
            sendPoint(pointScale(viewBox, seg.point(1)))
            # recieve()
            time.sleep(.2)
            recieve()
            # confirmPoint(2,0)
            # print(cxn.readline())

        if(type(seg)==quadType):
            counter = 0
            send(8)
            recieve()
            for x in range(0,8):
                print("Python sends point " + str(x) + " ",pointScale(viewBox, seg.point(x/8.)))
                sendPoint(pointScale(viewBox, seg.point(x/8.)))
                # recieve()
                time.sleep(.2)
                recieve()
                # print(cxn.readline())
                # confirmPoint(2,0)
            # confirmPoint(16,0)
            # print(cxn.readline())
        print('\n')

    time.sleep(1)

    print('\n')
