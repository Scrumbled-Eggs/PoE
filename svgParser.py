import serial
from svgpathtools import svg2paths, wsvg, Line, QuadraticBezier
from xml.dom import minidom

lineType = type(Line(start=(0+0j),end=(1,1j)))
quadType = type(QuadraticBezier(start=(0+0j), control=(.5+.5j), end=(1+1j)))

poesvg = open('PoE.svg', 'r')

doc = minidom.parse(poesvg)  # parseString also exists
viewBox = [el.getAttribute('viewBox') for el
        in doc.getElementsByTagName('svg')]
doc.unlink()
print(viewBox) # maps to 0 0 100 100 on our coordinates

def pointScale(viewBox, point):
    # This is bad but I'm under time pressure again
    minx = float(viewBox[0].split()[0])
    miny = float(viewBox[0].split()[1])
    maxx = float(viewBox[0].split()[2])
    maxy = float(viewBox[0].split()[3])
    xScale = 100/(maxx-minx)
    yScale = 100/(maxy-miny)
    scale = min(xScale, yScale)
    return(point.real *scale, point.imag*scale)

paths, attributes = svg2paths('PoE.svg')
for path in paths:
    for seg in path:
        if(type(seg)==lineType):
            print(pointScale(viewBox, seg.point(0)))
            print(pointScale(viewBox, seg.point(1)))
        if(type(seg)==quadType):
            for x in range(0,10):
                print(pointScale(viewBox, seg.point(x/10.)))
        print('\n')

# pointScale(viewBox, 't')
cxn = serial.Serial('/dev/tty0', baudrate=9600)

cxn.write('hi')