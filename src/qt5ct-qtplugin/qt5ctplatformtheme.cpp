/*
 * Copyright (c) 2014, Ilya Kotov <forkotov02@hotmail.ru>
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

#include <QVariant>
#include <QSettings>
#include <QGuiApplication>
#include <QScreen>
#include <QFont>
#include <QPalette>
#include <private/qfont_p.h>
#include <qt5ct/qt5ct.h>
#include "qt5ctplatformtheme.h"


//QT_QPA_PLATFORMTHEME=qt5ct

Qt5CTPlatformTheme::Qt5CTPlatformTheme()
{
    m_customPalette = 0;

    QSettings settings(Qt5CT::configFile(), QSettings::IniFormat);

    settings.beginGroup("Appearance");
    m_style = settings.value("style", "Fusion").toString();
    if(settings.value("custom_palette", false).toBool())
    {
        m_customPalette = new QPalette();
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
                m_customPalette->setColor(QPalette::Active, role, QColor(activeColors.at(i)));
                m_customPalette->setColor(QPalette::Inactive, role, QColor(inactiveColors.at(i)));
                m_customPalette->setColor(QPalette::Disabled, role, QColor(disabledColors.at(i)));
            }
        }
    }
    m_iconTheme = settings.value("icon_theme").toString();
    settings.endGroup();

    settings.beginGroup("Fonts");
    m_generalFont = settings.value("general", QPlatformTheme::font(QPlatformTheme::SystemFont)).value<QFont>();
    m_fixedFont = settings.value("fixed", QPlatformTheme::font(QPlatformTheme::FixedFont)).value<QFont>();
    settings.endGroup();

    QGuiApplication::setFont(m_generalFont); //apply font

}

Qt5CTPlatformTheme::~Qt5CTPlatformTheme()
{
    if(m_customPalette)
        delete m_customPalette;
}

const QPalette *Qt5CTPlatformTheme::palette(QPlatformTheme::Palette type) const
{
    if(m_customPalette)
        return m_customPalette;
    return QPlatformTheme::palette(type);
}

const QFont *Qt5CTPlatformTheme::font(QPlatformTheme::Font type) const
{
    if(type == QPlatformTheme::FixedFont)
        return &m_fixedFont;
    return &m_generalFont;
}

QVariant Qt5CTPlatformTheme::themeHint(QPlatformTheme::ThemeHint hint) const
{
    switch (hint)
    {
    case QPlatformTheme::SystemIconThemeName:
        return m_iconTheme;
    case QPlatformTheme::StyleNames:
        return QStringList() << m_style;
    case IconThemeSearchPaths:
        return Qt5CT::iconPaths();
    default:
        return QPlatformTheme::themeHint(hint);
    }
}
