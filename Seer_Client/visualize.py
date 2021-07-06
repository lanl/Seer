import k3d
import numpy as np
import random
import sys
!{sys.executable} -m pip install k3d


def drawPoints(points_pos, points_color, title, pointSize=0.1):
    """Visualizing point data"""
    plot = k3d.plot(name=title)
    plt_points = k3d.points(positions=points_pos.astype(np.float32), 
                            colors=points_color, 
                            point_size=pointSize, 
                            color_range = [0,1],
                            shader='flat')
    plot += plt_points
    plt_points.shader='3d'
    plot.display()