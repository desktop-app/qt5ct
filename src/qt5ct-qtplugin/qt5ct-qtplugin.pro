include(../../qt5ct.pri)

TEMPLATE = lib
TARGET = qt5ct
CONFIG += plugin

QT += gui-private

!equals(DISABLE_DBUS, 1):qtHaveModule(dbus):greaterThan(QT_MINOR_VERSION, 5) {
    greaterThan(QT_MINOR_VERSION, 7) {
        QT += theme_support-private
    } else {
        QT += platformsupport-private
    }
    QT += dbus
    message(D-Bus support: Enabled)
} else {
    message(D-Bus support: Disabled)
}

SOURCES += \
    main.cpp \
    qt5ctplatformtheme.cpp \
    ../qt5ct/qt5ct.cpp

!equals(DISABLE_WIDGETS,1) {
   QT += widgets
}

OTHER_FILES += qt5ct.json

INCLUDEPATH += ../

HEADERS += \
    qt5ctplatformtheme.h \
    ../qt5ct/qt5ct.h

target.path = $$PLUGINDIR/platformthemes
INSTALLS += target
