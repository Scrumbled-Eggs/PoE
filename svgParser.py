from svg.path import parse_path, Line
svgpath = 'M 100 100 L 300 100 L 200 300 z'
path = parse_path(svgpath)
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
	print('{' + str(startx) + ',' + str(starty) + '}')