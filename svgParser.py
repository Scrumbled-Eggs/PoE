import serial
from svgpathtools import svg2paths, wsvg, Line, QuadraticBezier
from xml.dom import minidom
import argparse

lineType = type(Line(start=(0+0j),end=(1,1j)))
quadType = type(QuadraticBezier(start=(0+0j), control=(.5+.5j), end=(1+1j)))

parser = argparse.ArgumentParser(description='Take a file and scale.')
parser.add_argument('filename')
args = parser.parse_args()

# Use the second solution on this stack overflow post to guarandtee better results

poesvg = open(args.filename, 'r')

doc = minidom.parse(poesvg)  # parseString also exists
viewBox = [el.getAttribute('viewBox') for el
        in doc.getElementsByTagName('svg')]
svgWidth = [el.getAttribute('width') for el
        in doc.getElementsByTagName('rect')]
svgHeight =[el.getAttribute('height') for el
        in doc.getElementsByTagName('rect')]

doc.unlink()
# print(viewBox) # maps to 0 0 100 100 on our coordinates
svgWidth = int(''.join(list(filter(str.isdigit, svgWidth[0]))))
svgHeight = int(''.join(list(filter(str.isdigit, svgHeight[0]))))
viewBox=[0,0,svgWidth, svgHeight]
outBox = [0,0,100,100]

def pointScale(svgBox, outBox, point):
    xScale = (outBox[2]-outBox[0])/(svgBox[2]-svgBox[0])
    yScale = (outBox[3]-outBox[1])/(svgBox[3]-svgBox[1])
    return(point.real *xScale, point.imag*yScale)

paths, attributes = svg2paths('PoE.svg')
for path in paths:
    for seg in path:
        if(type(seg)==lineType):
            for i in range(0,2):
                pointToSend = pointScale(viewBox, outBox, seg.point(i))
                # print(seg.point(i))
                print("{" + str(pointToSend[0]) + "," + str(pointToSend[1]) + "}")

        if(type(seg)==quadType):
            for x in range(0,10):
                pointToSend = pointScale(viewBox, outBox, seg.point(x/10.))
                print("{" + str(pointToSend[0]) + "," + str(pointToSend[1]) + "}")
