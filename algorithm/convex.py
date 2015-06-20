from pylab import *
import math


class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

if __name__ == '__main__':
    x = [ 5,  0,  8,  5,  9,  2,  8,  7,  0,  9]
    y = [ 8,  3,  6,  7,  4,  0,  5,  3,  1,  5]
    plot(x, y, 'ro')
    
    points = [None] * len(x)
    for i in range(0, 10):
        points[i] = Point(x[i], y[i])
   
    #find left bottom
    mini = points[0]
    for item in points:
        if item.y < mini.y or (item.y == mini.y and item.x < mini.x):
            mini = item

    points[0].x, mini.x = mini.x, points[0].x
    points[0].y, mini.y = mini.y, points[0].y 
    print points[0].x, points[0].y       
    #for i, point in enumerate(points):
    #    print i, point.x, point.y
      
   
    #sort
    for i in range(1, len(points)):
        for j in range(i+1, len(points)):
            dx = points[i].x - points[0].x
            dy = points[i].y - points[0].y
            dis = math.sqrt(dx * dx + dy * dy)
            cos = float(dx) / dis
            
            dx_j = points[j].x - points[0].x
            dy_j = points[j].y - points[0].y
            dis_j = math.sqrt(dx_j * dx_j + dy_j * dy_j)
            cos_j = float(dx_j) / dis_j
            if cos_j > cos or (cos_j == cos and dis_j < dis):
                points[i].x, points[j].x = points[j].x, points[i].x
                points[i].y, points[j].y = points[j].y, points[i].y
               
    for i, point in enumerate(points):
        print i,'--->', point.x, point.y
    
    stack = []
    stack.append(points[0])
    stack.append(points[1])
    stack.append(points[2])
    for i in range(3, len(points)):
        while len(stack) >= 2:
            o2, o1 = stack[-1], stack[-2]
            p1_x, p1_y = o2.x - o1.x, o2.y - o1.y
            p2_x, p2_y = points[i].x - o2.x, points[i].y - o2.y
            #vector p2 in the clock wise of p1
            if p2_x * p1_y - p2_y * p1_x > 0:
                stack.pop()
            else:
                stack.append(points[i])
                break
    cx,cy = [], []       
    for item in stack:
        cx.append(item.x)
        cy.append(item.y)
    cx.append(points[0].x)
    cy.append(points[0].y) 
    print cx, cy
    plot(cx, cy)
    show()
    