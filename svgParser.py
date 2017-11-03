from svg.path import parse_path, Path, Line, Arc, CubicBezier, QuadraticBezier
svgLinePath = 'M 100 100 L 300 100 L 200 300 z'
svgArcPath = ''
arcPath = Path(QuadraticBezier(300+100j, 200+200j, 200+300j))
print(arcPath.point(0))
print(arcPath.point(.25))
print(arcPath.point(.5))
print(arcPath.point(.75))
print(arcPath.point(1))
path = parse_path(svgLinePath)

# max is area is 100,100 to 300, 300
# draw area is 0 - 9

for el in path: 
        # print(el.point(0)) 
        # print(el.point(1))
        start = el.point(0)
        end = el.point(1)
        startx = start.real
        startx -= 100
        startx = startx* (9.0/200)
        starty = start.imag
        starty -= 100
        starty = starty* (9.0/200)
        #print('{' + str(startx) + ',' + str(starty) + '}')
