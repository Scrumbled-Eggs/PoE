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
print(viewBox) # maps to 0 0 100 100 on our coordinates
cxn = serial.Serial('/dev/ttyUSB0', baudrate=9600)

def pointScale(viewBox, point):
    # This is bad but I'm under time pressure again
    minx = float(viewBox[0].split()[0])
    miny = float(viewBox[0].split()[1])
    maxx = float(viewBox[0].split()[2])
    maxy = float(viewBox[0].split()[3])
    xScale = 100/(maxx-minx)
    yScale = 100/(maxy-miny)
    scale = min(xScale, yScale)
    return(int(point.real *scale), int(point.imag*scale))

time.sleep(.5)
print(cxn.readline())
print(cxn.readline())

paths, attributes = svg2paths('PoE.svg')
for path in paths:
    for seg in path:
        if(type(seg)==lineType):
            counter = 0
            print("Python sends point 0 ", pointScale(viewBox, seg.point(0)))
            cxn.write(struct.pack('i', pointScale(viewBox, seg.point(0))[0]))
            cxn.write(struct.pack('i', pointScale(viewBox, seg.point(0))[1]))
            while counter<2:
                while cxn.inWaiting():
                    print(cxn.readline())
                    counter += 1

            counter = 0
            print("Python sends point 1 ", pointScale(viewBox, seg.point(1)))
            cxn.write(struct.pack('i', pointScale(viewBox, seg.point(1))[0]))
            cxn.write(struct.pack('i', pointScale(viewBox, seg.point(1))[1]))
            while counter<2:
                while cxn.inWaiting():
                    print(cxn.readline())
                    counter += 1
            # cxn.write([pointScale(viewBox, seg.point(0))[0]])
            # cxn.write([pointScale(viewBox, seg.point(0))[1]])
            # print(pointScale(viewBox, seg.point(1)))
            # cxn.write([pointScale(viewBox, seg.point(1))[0]])
            # cxn.write([pointScale(viewBox, seg.point(1))[1]])
        if(type(seg)==quadType):
            counter = 0
            for x in range(0,8):
                print("Python sends point " + str(x) + " ",pointScale(viewBox, seg.point(x/8.)))
                cxn.write(struct.pack('i', pointScale(viewBox, seg.point(x/8.))[0]))
                cxn.write(struct.pack('i', pointScale(viewBox, seg.point(x/8.))[1]))
            while counter<16:
                while cxn.inWaiting():
                    print("Received point " + str(counter/2) + " ", cxn.readline())
                    # print(cxn.inWaiting())
                    counter += 1
            counter = 0
        print('\n')


# pointScale(viewBox, 't')


# cxn.write(['hi'])