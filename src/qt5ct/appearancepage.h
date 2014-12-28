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

#ifndef APPEARANCEPAGE_H
#define APPEARANCEPAGE_H

#include "tabpage.h"

namespace Ui {
class AppearancePage;
class PreviewForm;
}

class QStyle;

class AppearancePage : public TabPage
{
    Q_OBJECT

public:
    explicit AppearancePage(QWidget *parent = 0);
    ~AppearancePage();

    void writeSettings();

private slots:
    void on_styleComboBox_activated(const QString &text);
    void updatePalette();
    void on_changePaletteButton_clicked();

private:
    void readSettings();
    void setStyle(QWidget *w, QStyle *s);
    void setPalette(QWidget *w, QPalette p);
    Ui::AppearancePage *m_ui;
    QStyle *m_selectedStyle;
    QPalette m_customPalette;
    QWidget *m_previewWidget;
    Ui::PreviewForm *m_previewUi;
};

#endif // APPEARANCEPAGE_H
