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

#ifndef QSSPAGE_H
#define QSSPAGE_H

#include "tabpage.h"

namespace Ui {
class QSSPage;
}

class QListWidgetItem;
class QMenu;

class QSSPage : public TabPage
{
    Q_OBJECT

public:
    explicit QSSPage(QWidget *parent = 0);
    ~QSSPage();

    void writeSettings();

private slots:
    void on_qssListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *);
    void on_createButton_clicked();
    void on_editButton_clicked();
    void on_removeButton_clicked();
    void on_renameButton_clicked();
    void on_qssListWidget_customContextMenuRequested(const QPoint &pos);

private:
    void readSettings();
    void findStyleSheets(const QString &path);
    Ui::QSSPage *m_ui;
    QMenu *m_menu;
};

#endif // QSSPAGE_H
