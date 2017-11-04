/*
 * Copyright (c) 2014-2017, Ilya Kotov <forkotov02@ya.ru>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <QDir>
#include <QLocale>
#include <QLatin1String>
#include <QStandardPaths>
#include "qt5ct.h"

#ifndef QT5CT_DATADIR
#define QT5CT_DATADIR "/usr/share"
#endif


QString Qt5CT::configPath()
{
    return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + QLatin1String("/qt5ct");
}

QString Qt5CT::configFile()
{
    return configPath() + QLatin1String("/qt5ct.conf");
}

QStringList Qt5CT::iconPaths()
{
    QStringList paths;
    paths << QDir::homePath() + QLatin1String("/.icons");

    foreach (QString p, QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation))
    {
        paths << (p + QLatin1String("/icons"));
    }
    paths.removeDuplicates();

    //remove invalid
    foreach (QString p, paths)
    {
        if(!QDir(p).exists())
            paths.removeAll(p);
    }
    return paths;
}

QString Qt5CT::userStyleSheetPath()
{
    return configPath() + QLatin1String("/qss");
}

QStringList Qt5CT::sharedStyleSheetPaths()
{
    QStringList paths;
    foreach (QString p, QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation))
    {
        paths << (p + QLatin1String("/qt5ct/qss"));
    }
    paths << QLatin1String(QT5CT_DATADIR"/qt5ct/qss");
    paths.removeDuplicates();
    return paths;
}

QString Qt5CT::userColorSchemePath()
{
    return configPath() + QLatin1String("/colors");
}

QStringList Qt5CT::sharedColorSchemePaths()
{
    QStringList paths;
    foreach (QString p, QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation))
    {
        paths << (p + QLatin1String("/qt5ct/colors"));
    }
    paths << QLatin1String(QT5CT_DATADIR"/qt5ct/colors");
    paths.removeDuplicates();
    return paths;
}

QString Qt5CT::systemLanguageID()
{
#ifdef Q_OS_UNIX
    QByteArray v = qgetenv ("LC_ALL");
    if (v.isEmpty())
        v = qgetenv ("LC_MESSAGES");
    if (v.isEmpty())
        v = qgetenv ("LANG");
    if (!v.isEmpty())
        return QLocale (v).name();
#endif
    return  QLocale::system().name();
}
