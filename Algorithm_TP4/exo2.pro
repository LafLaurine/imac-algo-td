#-------------------------------------------------
#
# Project created by QtCreator 2019-02-03T21:14:48
#
#-------------------------------------------------

lessThan(QT_MAJOR_VERSION, 4): error("requires Qt>4")

QT       += core gui widgets

TARGET = Algorithm_TD4_exo2
TEMPLATE = app
CONFIG += c++11

SOURCES += \
		exo2.cpp \
		lib/mainwindow.cpp \
		lib/thread.cpp \
		lib/array.cpp \
		lib/binarytree.cpp \
		lib/heap.cpp \
		lib/tp3.cpp \
		lib/tp4.cpp \

HEADERS += \
		lib/mainwindow.h \
		lib/thread.h \
		lib/array.h \
		lib/binarytree.h \
		lib/heap.h \
		lib/tp3.h \
		lib/tp4.h \
