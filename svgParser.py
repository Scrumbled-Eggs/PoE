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
cxn.setDTR(True)
cxn.flush()
cxn.flushInput()

def pointScale(viewBox, point):
    # This is bad but I'm under time pressure again
    minx = float(viewBox[0].split()[0])
    miny = float(viewBox[0].split()[1])
    maxx = float(viewBox[0].split()[2])
    maxy = float(viewBox[0].split()[3])
    xScale = 1000/(maxx-minx)
    yScale = 1000/(maxy-miny)
    scale = min(xScale, yScale)
    return(str(int((point.real *scale))), str(int((point.imag*scale))))

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
        print(recievedVal)
        return None

    time.sleep(.1)
    # except:
    #     print(recievedVal.decode())


def sendPoint(point):
    cxn.flush()
    cxn.flushInput()
    cxn.write(point[0].encode('utf-8'))
    # send(point[0])
    time.sleep(.1)
    # send(point[1])
    time.sleep(.1)

def confirmPoint(point, pointNumber):
    hasSentCorrectly = False
    while not(hasSentCorrectly):
        print("Python sends point " + str(pointNumber) + " ",point)
        sendPoint(point)
        # time.sleep(.2)
        valRec = recieve()
        if valRec == None:
            print("ERROR!!!")
        elif tuple(valRec) != point:
            print("wrong point", tuple(valRec))
        elif tuple(valRec) == point:
            hasSentCorrectly = True
            print("Correct value has been confirmed")


# time.sleep(1.1)
# recieve()
# recieve()

paths, attributes = svg2paths('PoE.svg')
for path in paths:
    for seg in path:
        if(type(seg)==lineType):
            counter = 0
            for i in range(0,2):
                pointToSend = pointScale(viewBox, seg.point(i))
                confirmPoint(pointToSend, i)
                # sendPoint(pointToSend)


        if(type(seg)==quadType):
            for i in range(0,8):
                pointToSend = pointScale(viewBox, seg.point(i))
                confirmPoint(pointToSend, i)
        print('\n')
        time.sleep(1)
        cxn.flush()
        cxn.flushInput()

    print('a path Finished')
    cxn.flush()
    cxn.flushInput()
