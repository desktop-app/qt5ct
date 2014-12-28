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

#include <QSettings>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDir>
#include <QTreeWidgetItem>
#include <QImageReader>
#include <QDebug>
#include "qt5ct.h"
#include "iconthemepage.h"
#include "ui_iconthemepage.h"

IconThemePage::IconThemePage(QWidget *parent) :
    TabPage(parent),
    m_ui(new Ui::IconThemePage)
{
    m_ui->setupUi(this);
    loadThemes();
    readSettings();
}

IconThemePage::~IconThemePage()
{
    delete m_ui;
}

void IconThemePage::writeSettings()
{
    QSettings settings(Qt5CT::configFile(), QSettings::IniFormat);
    QTreeWidgetItem *item = m_ui->treeWidget->currentItem();
    if(item)
        settings.setValue("Appearance/icon_theme", item->data(3, Qt::UserRole));
}

void IconThemePage::readSettings()
{
    QSettings settings(Qt5CT::configFile(), QSettings::IniFormat);
    QString name = settings.value("Appearance/icon_theme").toString();

    if(name.isEmpty())
        return;

    for(int i = 0; i < m_ui->treeWidget->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem *item = m_ui->treeWidget->topLevelItem(i);
        if(item->data(3, Qt::UserRole).toString() == name)
        {
            m_ui->treeWidget->setCurrentItem(item);
            break;
        }
    }
}

void IconThemePage::loadThemes()
{
    QFileInfoList themeFileList;
    foreach(QString path, Qt5CT::iconPaths())
    {
        QDir dir(path);
        dir.setFilter(QDir::Dirs | QDir::NoDotDot | QDir::NoDot);
        foreach (QFileInfo info, dir.entryInfoList())
        {
            QDir themeDir(info.absoluteFilePath());
            themeDir.setFilter(QDir::Files);
            themeFileList << themeDir.entryInfoList(QStringList() << "index.theme");
        }
    }

    foreach(QFileInfo info, themeFileList)
    {
        loadTheme(info.absoluteFilePath());
    }
}

void IconThemePage::loadTheme(const QString &path)
{
    QSettings config(path, QSettings::IniFormat);

    config.beginGroup("Icon Theme");
    QStringList dirs = config.value("Directories").toStringList();
    if(dirs.isEmpty() || config.value("Hidden", false).toBool())
        return;
    QString name = config.value("Name").toString();
    QString comment = config.value("Comment").toString();
    QStringList inherits = config.value("Inherits").toStringList();
    config.endGroup();

    QStringList iconPaths;

    foreach (QString dir, dirs)
    {
        config.beginGroup(dir);
        if(config.value("Context").toString() == "Actions" && (config.value("Size").toInt() == 22
                                                               || config.value("Size").toInt() == 24))
        {
            iconPaths << QFileInfo(path).path() + "/" + dir;
            foreach (QString altDir, inherits)
            {
                iconPaths << QFileInfo(path).path() + "/../" + altDir + "/" + dir;
            }
        }
        config.endGroup();
    }

    if(iconPaths.isEmpty())
        return;

    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setIcon(0, loadIcon(iconPaths, "document-save"));
    item->setIcon(1, loadIcon(iconPaths, "document-print"));
    item->setIcon(2, loadIcon(iconPaths, "media-playback-stop"));
    item->setText(3, name);
    item->setData(3, Qt::UserRole, QFileInfo(path).path().section("/", -1));
    item->setToolTip(3, comment);
    item->setSizeHint(0, QSize(24,24));
    m_ui->treeWidget->addTopLevelItem(item);

    m_ui->treeWidget->resizeColumnToContents(0);
    m_ui->treeWidget->resizeColumnToContents(1);
    m_ui->treeWidget->resizeColumnToContents(2);
    m_ui->treeWidget->resizeColumnToContents(3);
}

QIcon IconThemePage::loadIcon(const QStringList &paths, const QString &name)
{
    foreach (QString path, paths)
    {
        QDir iconDir(path);
        iconDir.setFilter(QDir::Files);
        iconDir.setNameFilters(QStringList () << (name + ".*"));
        QFileInfoList iconList = iconDir.entryInfoList();
        if(!iconList.isEmpty())
            return QIcon(iconList.first().filePath());
    }
    return QIcon();
}
