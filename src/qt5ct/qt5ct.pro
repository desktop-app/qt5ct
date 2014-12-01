include(../../qt5ct.pri)

TEMPLATE = app

QT += gui-private core-private widgets

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    tabpage.cpp \
    appearancepage.cpp \
    fontspage.cpp \
    qt5ct.cpp \
    paletteeditdialog.cpp \
    iconthemepage.cpp

FORMS += \
    mainwindow.ui \
    appearancepage.ui \
    fontspage.ui \
    paletteeditdialog.ui \
    iconthemepage.ui

HEADERS += \
    mainwindow.h \
    tabpage.h \
    appearancepage.h \
    fontspage.h \
    qt5ct.h \
    paletteeditdialog.h \
    iconthemepage.h
