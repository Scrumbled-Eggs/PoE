import serial
from svgpathtools import svg2paths, wsvg, Line, QuadraticBezier
from xml.dom import minidom
import argparse
import re # for editing the cpp

# Define types for parsing SVG
lineType = type(Line(start=(0+0j),end=(1,1j)))
quadType = type(QuadraticBezier(start=(0+0j), control=(.5+.5j), end=(1+1j)))

# Define parse for CLI
parser = argparse.ArgumentParser(description='Take a file and scale.')
parser.add_argument('filename')
parser.add_argument('minx', type=int)
parser.add_argument('miny', type=int)
parser.add_argument('maxx', type=int)
parser.add_argument('maxy', type=int)
args = parser.parse_args()

# Get the user define output box
outBox = [args.minx,args.miny, args.maxx, args.maxy]

# Use the second solution on this stack overflow post to guarandtee better results
# https://stackoverflow.com/questions/13329125/removing-transforms-in-svg-files

poesvg = open(args.filename, 'r')

doc = minidom.parse(poesvg)  # parseString also exists

# Get the svg width and height from an SVG made using the SO toolchain
svgWidth = [el.getAttribute('width') for el
        in doc.getElementsByTagName('rect')] or [el.getAttribute('width') for el
                in doc.getElementsByTagName('svg')]
svgHeight =[el.getAttribute('height') for el
        in doc.getElementsByTagName('rect')] or [el.getAttribute('height') for el
                in doc.getElementsByTagName('svg')]
doc.unlink()

# Convert the svg shape into the box format
svgWidth = int(''.join(list(filter(str.isdigit, svgWidth[0]))))
svgHeight = int(''.join(list(filter(str.isdigit, svgHeight[0]))))
viewBox=[0,0,svgWidth, svgHeight]

# Initialize marker control variables
currentPosition = (0,0)
markerDown = True

# Initialize string for path
stringToReturn = ""
stringToReturn += "//PYTHONSTARTFLAG \n"

def pointScale(svgBox, outBox, point):
    ### Scale a point from the size/shape of svg box to the size/shape of outBox
    xScale = (outBox[2]-outBox[0])/(svgBox[2]-svgBox[0])
    yScale = (outBox[3]-outBox[1])/(svgBox[3]-svgBox[1])
    outPoint = (int(point.real *xScale) + outBox[0], int(point.imag*yScale)+outBox[1])
    # print(outPoint)
    assert outPoint[0] > 150 and outPoint[0] < 1100
    assert outPoint[1] > 0 and outPoint[1] < 1200
    return outPoint

paths, attributes = svg2paths(args.filename)

stringToReturn += "const int path[][2] =   {\n"

numPoints = 0

for path in paths:
    for seg in path:
        if currentPosition != seg.point(0):
            numPoints += 1
            # If the start point of a segment is off, pen up
            stringToReturn += "{-20,0},"
            stringToReturn += ('\n')
            markerDown = False

        if(type(seg)==lineType):
            for i in range(0,2):
                pointToSend = pointScale(viewBox, outBox, seg.point(i))
                stringToReturn += ("{" + str(pointToSend[0]) + "," + str(pointToSend[1]) + "},")
                numPoints += 1
                stringToReturn += ('\n')
                currentPosition = seg.point(i)
                if not(markerDown):
                    # If the marker is up, set it down
                    stringToReturn += "{-10,0},"
                    stringToReturn += ('\n')
                    markerDown = True
                    numPoints += 1

        if(type(seg)==quadType):
            for x in range(0,10):
                pointToSend = pointScale(viewBox, outBox, seg.point(x/10.))
                stringToReturn += ("{" + str(pointToSend[0]) + "," + str(pointToSend[1]) + "},")
                numPoints += 1
                stringToReturn += ('\n')
                currentPosition = seg.point(i)
                if not(markerDown):
                    # If the marker is up, set it down
                    stringToReturn += "{-10,0},"
                    stringToReturn += ('\n')
                    markerDown = True
                    numPoints += 1


stringToReturn += "{-20, 0},\n"
stringToReturn += "{init_pos.x, init_pos.y}\n"
numPoints += 2
stringToReturn += "};\n\n"
stringToReturn += "const int num_path = {};\n".format(numPoints)
stringToReturn +="//PYTHONENDFLAG"

# Read the current CPP file
with open('src/main.cpp') as f:
    cppData = f.read()

# Replace the path with the new path
cppData = re.sub('//PYTHONSTARTFLAG.*?//PYTHONENDFLAG',stringToReturn,cppData,flags=re.DOTALL)

# Write the cpp file again.
cppFile = open('src/main.cpp', 'w')
cppFile.write(cppData)
