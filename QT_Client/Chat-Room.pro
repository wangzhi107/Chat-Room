#-------------------------------------------------
#
# Project created by QtCreator 2020-04-09T09:20:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Chat-Room
TEMPLATE = app


SOURCES += main.cpp\
		mainwindow.cpp\
		client.c\
		recvthread.cpp

HEADERS  += mainwindow.h\
		client.h\
		recvthread.h

FORMS    += mainwindow.ui
