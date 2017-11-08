# from svg.path import parse_path, Path, Line, Arc, CubicBezier, QuadraticBezier
# svgLinePath = 'M 100 100 L 300 100 L 200 300 z'
# svgArcPath = ''
# arcPath = Path(QuadraticBezier(300+100j, 200+200j, 200+300j))
# print(arcPath.point(0))
# print(arcPath.point(.25))
# print(arcPath.point(.5))
# print(arcPath.point(.75))
# print(arcPath.point(1))
# path = parse_path(svgLinePath)

# max is area is 100,100 to 300, 300
# draw area is 0 - 9

# from xml.dom import minidom

# poesvg = open('PoE.svg', 'r')

# doc = minidom.parse(poesvg)  # parseString also exists
# path_strings = [path.getAttribute('d') for path
#         in doc.getElementsByTagName('path')]
# doc.unlink()

# import re
# mode = 0
# toSend = ""

import serial
# path = parse_path(path_strings)
# print(path)

from svgpathtools import svg2paths, wsvg

paths, attributes = svg2paths('PoE.svg')
for path in paths:
    print(path)
    print('\n')

# cxn = serial.Serial('/dev/tty.usbserial', baudrate=9600)

# for thing in path_strings:
#     ops = re.sub( r"(\W+[,])", r" \1", thing).split()
#     for op in ops:
#         if len(op) == 1:
#             mode = op
#             toSend = mode
#             # Send the mode the arduino is in
#         elif mode = m:
#             toSend = op
#             # Send a signle point to move to
#         elif mode = q:

#         print(op)
#         # If op == letter and mode == m, set zreturn = current_position

#         # cxn.write([op])


# for el in path: 
#     # print(el.point(0)) 
#     # print(el.point(1))
#     start = el.point(0)
#     end = el.point(1)
#     startx = start.real
#     startx -= 100
#     startx = startx* (9.0/200)
#     starty = start.imag
#     starty -= 100
#     starty = starty* (9.0/200)
    #print('{' + str(startx) + ',' + str(starty) + '}')
