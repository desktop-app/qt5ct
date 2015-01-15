include(../../qt5ct.pri)

TEMPLATE = lib
TARGET = qt5ct
CONFIG += plugin

QT += gui-private core-private widgets-private

SOURCES += \
    main.cpp \
    qt5ctplatformtheme.cpp \
    ../qt5ct/qt5ct.cpp



OTHER_FILES += qt5ct.json

INCLUDEPATH += ../

HEADERS += \
    qt5ctplatformtheme.h \
    ../qt5ct/qt5ct.h

target.path = $$PLUGINDIR
INSTALLS += target

