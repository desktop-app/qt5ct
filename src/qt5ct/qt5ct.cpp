/*
 * Copyright (c) 2014-2015, Ilya Kotov <forkotov02@hotmail.ru>
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
#include "qt5ct.h"


QString Qt5CT::configPath()
{
    return QDir::homePath() + "/.config/qt5ct/";
}

QString Qt5CT::configFile()
{
    return configPath() + "qt5ct.conf";
}

QStringList Qt5CT::iconPaths()
{
    QString xdgDataDirs = qgetenv("XDG_DATA_DIRS");

    QStringList paths;
    paths << QDir::homePath() + "/.icons/";

    if(xdgDataDirs.isEmpty())
    {
        paths << "/usr/share/icons";
        paths << "/usr/local/share/icons";
    }
    else
    {
        foreach (QString p, xdgDataDirs.split(":"))
            paths << QDir(p + "/icons/").absolutePath();
    }
    paths << "/usr/share/pixmaps";
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
    return configPath() + "qss/";
}

QString Qt5CT::sharedStyleSheetPath()
{
    return "/usr/share/qt5ct/qss/"; //TODO use installation prefix
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
