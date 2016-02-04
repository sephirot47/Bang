TEMPLATE = app
CONFIG += console
CONFIG += app_bundle

QT_VERSION = 4

SOURCES += \
    View/Canvas.cpp \
    Domain/main.cpp \
    Domain/Shader.cpp \
    Domain/Logger.cpp

OBJECTS_DIR = obj/

include(deployment.pri)
qtcAddDeployment()

QT += core gui opengl

OTHER_FILES += \
    .gitignore \
    run.sh \
    updateUI.sh

CONFIG += qt

QMAKE_CXXFLAGS += --std=c++11

LIBS += -L/usr/lib/*.so \
        -L/usr/lib/qt4 \
        -L/usr/local/lib \
        -lGLEW

HEADERS += \
    View/Canvas.h \
    Domain/Shader.h \
    Domain/IToString.h \
    Domain/Idable.h \
    Domain/Logger.h

