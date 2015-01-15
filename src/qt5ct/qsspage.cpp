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
#include <QDir>
#include "qt5ct.h"
#include "qsspage.h"
#include "ui_qsspage.h"

#define QSS_FULL_PATH_ROLE (Qt::ItemDataRole(Qt::UserRole))
#define QSS_WRITABLE_ROLE (Qt::ItemDataRole(Qt::UserRole + 1))

QSSPage::QSSPage(QWidget *parent) :
    TabPage(parent),
    m_ui(new Ui::QSSPage)
{
    m_ui->setupUi(this);
    QDir("/").mkpath(Qt5CT::userStyleSheetPath());

    readSettings();
}

QSSPage::~QSSPage()
{
    delete m_ui;
}

void QSSPage::writeSettings()
{
    QStringList styleSheets;
    QSettings settings(Qt5CT::configFile(), QSettings::IniFormat);

    for(int i = 0; i < m_ui->qssListWidget->count(); ++i)
    {
        QListWidgetItem *item = m_ui->qssListWidget->item(i);
        if(item->checkState() == Qt::Checked)
            styleSheets << item->data(QSS_FULL_PATH_ROLE).toString();
    }

    settings.setValue("Interface/stylesheets", styleSheets);
}

void QSSPage::readSettings()
{
    //load stylesheets
    m_ui->qssListWidget->clear();
    findStyleSheets(Qt5CT::userStyleSheetPath());
    findStyleSheets(Qt5CT::sharedStyleSheetPath());

    QSettings settings(Qt5CT::configFile(), QSettings::IniFormat);
    QStringList styleSheets = settings.value("Interface/stylesheets").toStringList();
    for(int i = 0; i < m_ui->qssListWidget->count(); ++i)
    {
        QListWidgetItem *item = m_ui->qssListWidget->item(i);
        if(styleSheets.contains(item->data(QSS_FULL_PATH_ROLE).toString()))
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);
    }
}

void QSSPage::findStyleSheets(const QString &path)
{
    QDir dir(path);
    dir.setFilter(QDir::Files);
    dir.setNameFilters(QStringList() << "*.qss");

    foreach (QFileInfo info, dir.entryInfoList())
    {
        QListWidgetItem *item = new QListWidgetItem(info.fileName(),  m_ui->qssListWidget);
        item->setToolTip(info.filePath());
        item->setData(QSS_FULL_PATH_ROLE, info.filePath());
        item->setData(QSS_WRITABLE_ROLE, info.isWritable());
    }
}
