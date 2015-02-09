#!/bin/sh

MYDIR=$(dirname $0)

export QT_DEBUG_PLUGINS=1
export QT_QPA_PLATFORMTHEME=qt5ct
export QT_QPA_PLATFORM_PLUGIN_PATH=$(dirname $0)/src/qt5ct-qtplugin
$(dirname $0)/src/qt5ct/qt5ct
