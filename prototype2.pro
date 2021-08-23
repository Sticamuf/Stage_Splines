QT       += core gui opengl printsupport svg

INCLUDEPATH += C:\Users\celad\OneDrive\Documents\QtProjects\prototype2\
LIBS += -C:\Users\celad\OneDrive\Documents\QtProjects\prototype2 -l
JAVADOC_BANNER = YES

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    alglibinternal.cpp \
    alglibmisc.cpp \
    ap.cpp \
    glwidget.cpp \
    individual.cpp \
    integration.cpp \
    interpolation.cpp \
    linalg.cpp \
    main.cpp \
    mainwindow.cpp \
    optimization.cpp \
    point.cpp \
    population.cpp \
    qcustomplot.cpp \
    solvers.cpp \
    specialfunctions.cpp \
    spline.cpp \
    window.cpp

HEADERS += \
    alglibinternal.h \
    alglibmisc.h \
    ap.h \
    glwidget.h \
    individual.h \
    integration.h \
    interpolation.h \
    linalg.h \
    mainwindow.h \
    optimization.h \
    point.h \
    population.h \
    qcustomplot.h \
    solvers.h \
    specialfunctions.h \
    spline.h \
    window.h

FORMS += \
    mainwindow.ui

LIBS += -lopengl32 -lglu32

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
