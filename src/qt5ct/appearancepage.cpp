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

#include <QStyleFactory>
#include <QMdiSubWindow>
#include <QSettings>
#include "qt5ct.h"
#include "appearancepage.h"
#include "paletteeditdialog.h"
#include "ui_appearancepage.h"
#include "ui_previewform.h"

AppearancePage::AppearancePage(QWidget *parent) :
    TabPage(parent),
    m_ui(new Ui::AppearancePage)
{
    m_selectedStyle = 0;

    m_ui->setupUi(this);
    m_ui->styleComboBox->addItems(QStyleFactory::keys());

    connect(m_ui->paletteComboBox, SIGNAL(activated(int)), SLOT(updatePalette()));
    connect(m_ui->customPaletteButton, SIGNAL(clicked()), SLOT(updatePalette()));
    connect(m_ui->defaultPaletteButton, SIGNAL(clicked()), SLOT(updatePalette()));

    m_previewWidget = new QWidget(this);
    m_previewUi = new Ui::PreviewForm();
    m_previewUi->setupUi(m_previewWidget);
    QMdiSubWindow *w = m_ui->mdiArea->addSubWindow(m_previewWidget, Qt::CustomizeWindowHint
                                                   | Qt::WindowMinMaxButtonsHint
                                                   | Qt::WindowTitleHint);
    w->move(10, 10);

    readSettings();
}

AppearancePage::~AppearancePage()
{
    if(m_selectedStyle)
        delete m_selectedStyle;
    delete m_ui;
    delete m_previewUi;
}

void AppearancePage::writeSettings()
{
    QSettings settings(Qt5CT::configFile(), QSettings::IniFormat);
    settings.beginGroup("Appearance");
    settings.setValue("style", m_ui->styleComboBox->currentText());
    settings.setValue("custom_palette", m_ui->customPaletteButton->isChecked());

    QStringList activeColors, inactiveColors, disabledColors;
    for (int i = 0; i < QPalette::NColorRoles; i++)
    {
        QPalette::ColorRole role = QPalette::ColorRole(i);
        activeColors << m_customPalette.color(QPalette::Active, role).name();
        inactiveColors << m_customPalette.color(QPalette::Inactive, role).name();
        disabledColors << m_customPalette.color(QPalette::Disabled, role).name();
    }

    settings.setValue("active_colors",activeColors);
    settings.setValue("inactive_colors",inactiveColors);
    settings.setValue("disabled_colors",disabledColors);

    settings.endGroup();
}

void AppearancePage::on_styleComboBox_activated(const QString &text)
{
    QStyle *style = QStyleFactory::create(text);
    if(!style)
        return;
    setStyle(m_previewWidget, style);

    if(m_selectedStyle)
        delete m_selectedStyle;
    m_selectedStyle = style;

    updatePalette();
}

void AppearancePage::updatePalette()
{
    if(!m_selectedStyle)
        return;

    QPalette previewPalette = palette();
    QPalette currentPalette = m_ui->customPaletteButton->isChecked() ?
                m_customPalette : m_selectedStyle->standardPalette();
    QPalette::ColorGroup colorGroup = QPalette::Disabled;

    if(m_ui->paletteComboBox->currentIndex() == 0)
    {
        colorGroup = QPalette::Active;
    }
    else if(m_ui->paletteComboBox->currentIndex() == 1)
    {
        colorGroup = QPalette::Inactive;
    }

    for (int i = 0; i < QPalette::NColorRoles; i++)
    {
        QPalette::ColorRole role = QPalette::ColorRole(i);
        previewPalette.setColor(QPalette::Active, role, currentPalette.color(colorGroup, role));
        previewPalette.setColor(QPalette::Inactive, role, currentPalette.color(colorGroup, role));
    }

    setPalette(m_ui->mdiArea, previewPalette);
}

void AppearancePage::readSettings()
{
    QSettings settings(Qt5CT::configFile(), QSettings::IniFormat);
    settings.beginGroup("Appearance");
    QString style = settings.value("style", "Fusion").toString();
    m_ui->styleComboBox->setCurrentText(style);

    m_ui->customPaletteButton->setChecked(settings.value("custom_palette", false).toBool());
    QStringList activeColors = settings.value("active_colors").toStringList();
    QStringList inactiveColors = settings.value("inactive_colors").toStringList();
    QStringList disabledColors = settings.value("disabled_colors").toStringList();
    if(activeColors.count() == QPalette::NColorRoles &&
            inactiveColors.count() == QPalette::NColorRoles &&
            disabledColors.count() == QPalette::NColorRoles)
    {
        for (int i = 0; i < QPalette::NColorRoles; i++)
        {
            QPalette::ColorRole role = QPalette::ColorRole(i);
            m_customPalette.setColor(QPalette::Active, role, QColor(activeColors.at(i)));
            m_customPalette.setColor(QPalette::Inactive, role, QColor(inactiveColors.at(i)));
            m_customPalette.setColor(QPalette::Disabled, role, QColor(disabledColors.at(i)));
        }
    }
    else
    {
        m_customPalette = palette(); //load fallback palette
    }

    on_styleComboBox_activated(m_ui->styleComboBox->currentText());

    settings.endGroup();
}

void AppearancePage::setStyle(QWidget *w, QStyle *s)
{
    foreach (QObject *o, w->children())
    {
        if(o->isWidgetType())
        {
            setStyle(qobject_cast<QWidget *>(o), s);
        }
    }
    w->setStyle(s);
}

void AppearancePage::setPalette(QWidget *w, QPalette p)
{
    foreach (QObject *o, w->children())
    {
        if(o->isWidgetType())
        {
            setPalette(qobject_cast<QWidget *>(o), p);
        }
    }
    w->setPalette(p);
}

void AppearancePage::on_changePaletteButton_clicked()
{
    PaletteEditDialog d(m_customPalette, m_selectedStyle, this);
    if(d.exec() == QDialog::Accepted)
    {
        m_customPalette = d.selectedPalette();
        updatePalette();
    }
}
