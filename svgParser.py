import serial
from svgpathtools import svg2paths, wsvg, Line, QuadraticBezier
from xml.dom import minidom
import argparse
import re # for editing the cpp

lineType = type(Line(start=(0+0j),end=(1,1j)))
quadType = type(QuadraticBezier(start=(0+0j), control=(.5+.5j), end=(1+1j)))

parser = argparse.ArgumentParser(description='Take a file and scale.')
parser.add_argument('filename')
parser.add_argument('minx', type=int)
parser.add_argument('miny', type=int)
parser.add_argument('maxx', type=int)
parser.add_argument('maxy', type=int)
args = parser.parse_args()

outBox = [args.minx,args.miny, args.maxx, args.maxy]

# Use the second solution on this stack overflow post to guarandtee better results
# https://stackoverflow.com/questions/13329125/removing-transforms-in-svg-files

poesvg = open(args.filename, 'r')

doc = minidom.parse(poesvg)  # parseString also exists
viewBox = [el.getAttribute('viewBox') for el
        in doc.getElementsByTagName('svg')]
svgWidth = [el.getAttribute('width') for el
        in doc.getElementsByTagName('rect')]
svgHeight =[el.getAttribute('height') for el
        in doc.getElementsByTagName('rect')]

doc.unlink()
svgWidth = int(''.join(list(filter(str.isdigit, svgWidth[0]))))
svgHeight = int(''.join(list(filter(str.isdigit, svgHeight[0]))))
viewBox=[0,0,svgWidth, svgHeight]

currentPosition = (0,0)
markerDown = True

stringToReturn = ""
stringToReturn += "//PYTHONSTARTFLAG \n"

def pointScale(svgBox, outBox, point):
    xScale = (outBox[2]-outBox[0])/(svgBox[2]-svgBox[0])
    yScale = (outBox[3]-outBox[1])/(svgBox[3]-svgBox[1])
    return(int(point.real *xScale), int(point.imag*yScale))

paths, attributes = svg2paths('PoE.svg')
for path in paths:
    for seg in path:

        if currentPosition != seg.point(0):
            # If the start point of a segment is off, pen up
            stringToReturn += "{-10,0}"
            markerDown = False

        if(type(seg)==lineType):
            for i in range(0,2):
                pointToSend = pointScale(viewBox, outBox, seg.point(i))
                stringToReturn += ("{" + str(pointToSend[0]) + "," + str(pointToSend[1]) + "}")
                stringToReturn += ('\n')
                currentPosition = seg.point(i)
                if !markerDown:
                    # If the marker is up, set it down
                    stringToReturn += "{-20,0}"

        if(type(seg)==quadType):
            for x in range(0,10):
                pointToSend = pointScale(viewBox, outBox, seg.point(x/10.))
                stringToReturn += ("{" + str(pointToSend[0]) + "," + str(pointToSend[1]) + "}")
                stringToReturn += ('\n')
                currentPosition = seg.point(i)
                if !markerDown:
                    # If the marker is up, set it down
                    stringToReturn += "{-20,0}"

stringToReturn +="//PYTHONENDFLAG"

with open('src/main.cpp') as f:
    cppData = f.read()
cppData = re.sub('//PYTHONSTARTFLAG.*?//PYTHONENDFLAG',stringToReturn,cppData,flags=re.DOTALL)

cppFile = open('src/main.cpp', 'w')
cppFile.write(cppData)
