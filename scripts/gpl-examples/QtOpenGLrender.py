#!/usr/bin/env python

"""Rendering pyPolyCSG results using Qt via PySide, and OpenGL."""

# """PySide port of the opengl/hellogl example from Qt v4.x"""
# This example started as the 'hellogl.py' example from the PySide project
# found here:
# http://qt.gitorious.org/pyside/pyside-examples/blobs/master/examples/opengl/hellogl.py
# That original example is licensed under GPLv2, as is this derivitive work.
#
# In the original example, the Trolltech logo is constructed in 3D
# geometry and rendered in a Qt widget using OpenGL.
# This program incorporates the following changes:
#  * makeObject() method: The Trolltech Qt logo geometry is removed,
#    and is replaced with geometry constructed using pyPolyCSG. 
#  * The paingGL() callback has been modified to render the
#    pyPolyCSG mesh using OpenGL calls.
#
# Example author: Dave Curtis davecurtis@sonic.net
#
# I want to gratefully acknowledge significant help from Emilian Doganov
# of 11D Labs who patiently explained how to use the pyOpenGL calls
# needed for rendering.
#
# Modifications Copyright (C) 2012 David B. Curtis. All rights reserved.
# Original work contained a notice of license but no copyright statement.

import sys
import math
from PySide import QtCore, QtGui, QtOpenGL

# DBC: added imports
import pyPolyCSG as csg
import numpy 

try:
    from OpenGL import GL
except ImportError:
    app = QtGui.QApplication(sys.argv)
    QtGui.QMessageBox.critical(None, "OpenGL hellogl",
                            "PyOpenGL must be installed to run this example.",
                            QtGui.QMessageBox.Ok | QtGui.QMessageBox.Default,
                            QtGui.QMessageBox.NoButton)
    sys.exit(1)


class Window(QtGui.QWidget):
    def __init__(self, parent=None):
        QtGui.QWidget.__init__(self, parent)

        self.glWidget = GLWidget()

        self.xSlider = self.createSlider(QtCore.SIGNAL("xRotationChanged(int)"),
                                         self.glWidget.setXRotation)
        self.ySlider = self.createSlider(QtCore.SIGNAL("yRotationChanged(int)"),
                                         self.glWidget.setYRotation)
        self.zSlider = self.createSlider(QtCore.SIGNAL("zRotationChanged(int)"),
                                         self.glWidget.setZRotation)

        mainLayout = QtGui.QHBoxLayout()
        mainLayout.addWidget(self.glWidget)
        mainLayout.addWidget(self.xSlider)
        mainLayout.addWidget(self.ySlider)
        mainLayout.addWidget(self.zSlider)
        self.setLayout(mainLayout)

        self.xSlider.setValue(15 * 16)
        self.ySlider.setValue(345 * 16)
        self.zSlider.setValue(0 * 16)

        self.setWindowTitle(self.tr("Hello GL"))

    def createSlider(self, changedSignal, setterSlot):
        slider = QtGui.QSlider(QtCore.Qt.Vertical)

        slider.setRange(0, 360 * 16)
        slider.setSingleStep(16)
        slider.setPageStep(15 * 16)
        slider.setTickInterval(15 * 16)
        slider.setTickPosition(QtGui.QSlider.TicksRight)

        self.glWidget.connect(slider, QtCore.SIGNAL("valueChanged(int)"), setterSlot)
        self.connect(self.glWidget, changedSignal, slider, QtCore.SLOT("setValue(int)"))

        return slider


class GLWidget(QtOpenGL.QGLWidget):
    def __init__(self, parent=None):
        QtOpenGL.QGLWidget.__init__(self, parent)

        self.object = (None,None,None)
        self.xRot = 0
        self.yRot = 0
        self.zRot = 0

        self.lastPos = QtCore.QPoint()

        self.trolltechGreen = QtGui.QColor.fromCmykF(0.40, 0.0, 1.0, 0.0)
        self.trolltechPurple = QtGui.QColor.fromCmykF(0.39, 0.39, 0.0, 0.0)

    def xRotation(self):
        return self.xRot

    def yRotation(self):
        return self.yRot

    def zRotation(self):
        return self.zRot

    def minimumSizeHint(self):
        return QtCore.QSize(50, 50)

    def sizeHint(self):
        return QtCore.QSize(400, 400)

    def setXRotation(self, angle):
        angle = self.normalizeAngle(angle)
        if angle != self.xRot:
            self.xRot = angle
            self.emit(QtCore.SIGNAL("xRotationChanged(int)"), angle)
            self.updateGL()

    def setYRotation(self, angle):
        angle = self.normalizeAngle(angle)
        if angle != self.yRot:
            self.yRot = angle
            self.emit(QtCore.SIGNAL("yRotationChanged(int)"), angle)
            self.updateGL()

    def setZRotation(self, angle):
        angle = self.normalizeAngle(angle)
        if angle != self.zRot:
            self.zRot = angle
            self.emit(QtCore.SIGNAL("zRotationChanged(int)"), angle)
            self.updateGL()

    def initializeGL(self):
        self.qglClearColor(self.trolltechPurple.darker())
        self.object = self.makeObject()
        GL.glShadeModel(GL.GL_FLAT)
        GL.glEnable(GL.GL_DEPTH_TEST)
        GL.glEnable(GL.GL_CULL_FACE) 
        
    def paintGL(self):
        GL.glClear(GL.GL_COLOR_BUFFER_BIT | GL.GL_DEPTH_BUFFER_BIT)
        GL.glLoadIdentity()
        GL.glTranslated(0.0, 0.0, -10.0)
        GL.glRotated(self.xRot / 16.0, 1.0, 0.0, 0.0)
        GL.glRotated(self.yRot / 16.0, 0.0, 1.0, 0.0)
        GL.glRotated(self.zRot / 16.0, 0.0, 0.0, 1.0)
        # DBC: Modifications from hellogl.py below this line.
        scalefactor = .1
        vertices, triangles, colors = self.object
        GL.glEnableClientState(GL.GL_VERTEX_ARRAY)
        GL.glScaled(scalefactor, scalefactor, scalefactor)
        GL.glEnableClientState(GL.GL_COLOR_ARRAY);
        GL.glColorPointerf(colors)
        GL.glVertexPointerf(vertices)
        GL.glDrawElementsui(GL.GL_TRIANGLES, triangles)
	
    def resizeGL(self, width, height):
        side = min(width, height)
        GL.glViewport((width - side) / 2, (height - side) / 2, side, side)
        GL.glMatrixMode(GL.GL_PROJECTION)
        GL.glLoadIdentity()
        GL.glOrtho(-0.5, +0.5, +0.5, -0.5, 4.0, 15.0)
        GL.glMatrixMode(GL.GL_MODELVIEW)

    def mousePressEvent(self, event):
        self.lastPos = QtCore.QPoint(event.pos())

    def mouseMoveEvent(self, event):
        dx = event.x() - self.lastPos.x()
        dy = event.y() - self.lastPos.y()

        if event.buttons() & QtCore.Qt.LeftButton:
            self.setXRotation(self.xRot + 8 * dy)
            self.setYRotation(self.yRot + 8 * dx)
        elif event.buttons() & QtCore.Qt.RightButton:
            self.setXRotation(self.xRot + 8 * dy)
            self.setZRotation(self.zRot + 8 * dx)

        self.lastPos = QtCore.QPoint(event.pos())

    # DBC: makeObject gutted and replaced.
    def makeObject(self):
        # Create a slab.
        self.polyBlock = csg.box(6,2,4,True)
        # Punch a hole in it.
        cyl = csg.cylinder(1,4,True)
        self.polyBlock -= cyl
        # Retrieve the vertices and triangles from the
        # pyPolyCSG mesh, and condition the numpy arrays
        # for rendering in the paingGL() callback.
        vertices = self.polyBlock.get_vertices()
        triangles = self.polyBlock.get_triangles().astype('uint16').flatten()
        # Make up some colors for the vertices.
        colors = numpy.array(len(vertices) * [[.7, .7, .7, .999]])
        # Return a tuple of (vertices, triangles, colors) 
        return (vertices,triangles, colors)
        
    def normalizeAngle(self, angle):
        while angle < 0:
            angle += 360 * 16
        while angle > 360 * 16:
            angle -= 360 * 16
        return angle


if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    window = Window()
    window.show()
    sys.exit(app.exec_())
