#-------------------------------------------------
#
# Project created by QtCreator 2019-01-04T14:47:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = virtualBoundary
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    localglcm.cpp \
    linesfilter2.cpp \
    regionanalyse.cpp

HEADERS  += mainwindow.h \
    func.h

FORMS    += mainwindow.ui

#OTHER_FILES += \
#    classify.py

#INCLUDEPATH +=include\
#           include\opencv\
#           include\opencv2\

#LIBS += $$PWD\lib\libopencv_*.dll.a

##need change to opencv2.4.9
INCLUDEPATH +=M:\opencv\opencv\build\include

#INCLUDEPATH += -I D:\programing\python3\include

#LIBS +=-LD:\programing\python3\libs\ -lpython3

##need change to opencv2.4.9
#LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_ml249d.lib
#LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_calib3d249d.lib
#LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_contrib249d.lib
#LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_core249d.lib
#LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_features2d249d.lib
#LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_flann249d.lib
#LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_gpu249d.lib
#LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_highgui249d.lib
#LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_imgproc249d.lib
#LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_legacy249d.lib
#LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_objdetect249d.lib
#LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_ts249d.lib
#LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_video249d.lib
#LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_nonfree249d.lib
#LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_ocl249d.lib
#LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_photo249d.lib
#LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_stitching249d.lib
#LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_superres249d.lib
#LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_videostab249d.lib
#LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_ml249d.lib

LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_ml249.lib
LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_calib3d249.lib
LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_contrib249.lib
LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_core249.lib
LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_features2d249.lib
LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_flann249.lib
LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_gpu249.lib
LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_highgui249.lib
LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_imgproc249.lib
LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_legacy249.lib
LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_objdetect249.lib
LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_ts249.lib
LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_video249.lib
LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_nonfree249.lib
LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_ocl249.lib
LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_photo249.lib
LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_stitching249.lib
LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_superres249.lib
LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_videostab249.lib
LIBS+=M:\opencv\opencv\build\x86\vc11\lib\opencv_ml249.lib
