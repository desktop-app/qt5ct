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

#include <QVariant>
#include <QSettings>
#include <QGuiApplication>
#include <QDebug>
#include <QScreen>
#include <QFont>
#include <QPalette>
#include <QTimer>
#include <QIcon>
#ifdef QT_WIDGETS_LIB
#include <QStyle>
#include <QApplication>
#include <QWidget>
#endif
#include <QFile>
#include <QFileSystemWatcher>

#include <qt5ct/qt5ct.h>
#include "qt5ctproxystyle.h"
#include "qt5ctplatformtheme.h"

//QT_QPA_PLATFORMTHEME=qt5ct

Qt5CTPlatformTheme::Qt5CTPlatformTheme()
{
    m_customPalette = 0;
    readSettings();
    QMetaObject::invokeMethod(this, "applySettings", Qt::QueuedConnection);
#ifdef QT_WIDGETS_LIB
    QMetaObject::invokeMethod(this, "createFSWatcher", Qt::QueuedConnection);
    //apply custom style hints before creating QApplication
    //using Fusion style should avoid problems with some styles like qtcurve
    QApplication::setStyle(new Qt5CTProxyStyle("Fusion"));
#endif
    QGuiApplication::setFont(m_generalFont);
    qDebug("using qt5ct plugin");
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
    case QPlatformTheme::CursorFlashTime:
        return m_cursorFlashTime;
    case MouseDoubleClickInterval:
        return m_doubleClickInterval;
    case QPlatformTheme::SystemIconThemeName:
        return m_iconTheme;
    case QPlatformTheme::StyleNames:
        return QStringList() << m_style;
    case QPlatformTheme::IconThemeSearchPaths:
        return Qt5CT::iconPaths();
    case DialogButtonBoxLayout:
        return m_buttonBoxLayout;
    case QPlatformTheme::UiEffects:
        return m_uiEffects;
    default:
        return QPlatformTheme::themeHint(hint);
    }
}

void Qt5CTPlatformTheme::applySettings()
{
#ifdef QT_WIDGETS_LIB
    if(hasWidgets())
    {
        //do not override proxy style (fixes crash in qupzilla)
        QProxyStyle *proxyStyle = qobject_cast<QProxyStyle *>(qApp->style());
        proxyStyle ? proxyStyle->setBaseStyle(0) : qApp->setStyle(new Qt5CTProxyStyle(m_style));
        qApp->setFont(m_generalFont);
        if(m_customPalette)
            qApp->setPalette(*m_customPalette);
        else
            qApp->setPalette(qApp->style()->standardPalette());

        if(m_prevStyleSheet == qApp->styleSheet())
            qApp->setStyleSheet(m_userStyleSheet);
        else
            qDebug("qt5ct: custom style sheet is disabled");
        m_prevStyleSheet = m_userStyleSheet;
    }
#endif
    QGuiApplication::setFont(m_generalFont); //apply font
    QIcon::setThemeName(m_iconTheme); //apply icons
    if(m_customPalette)
        QGuiApplication::setPalette(*m_customPalette); //apply palette

#ifdef QT_WIDGETS_LIB
    if(hasWidgets())
    {
        foreach (QWidget *w, qApp->allWidgets())
        {
            QEvent e(QEvent::ThemeChange);
            QApplication::sendEvent(w, &e);
        }
    }
#endif
}

#ifdef QT_WIDGETS_LIB
void Qt5CTPlatformTheme::createFSWatcher()
{
    QFileSystemWatcher *watcher = new QFileSystemWatcher(this);
    watcher->addPath(Qt5CT::configPath());

    QTimer *timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(3000);
    connect(watcher, SIGNAL(directoryChanged(QString)), timer, SLOT(start()));
    connect(timer, SIGNAL(timeout()), SLOT(updateSettings()));
}

void Qt5CTPlatformTheme::updateSettings()
{
    qDebug("Qt5CTPlatformTheme: updating settings..");
    readSettings();
    applySettings();
}
#endif

void Qt5CTPlatformTheme::readSettings()
{
    if(m_customPalette)
    {
        delete m_customPalette;
        m_customPalette = 0;
    }

    QSettings settings(Qt5CT::configFile(), QSettings::IniFormat);

    settings.beginGroup("Appearance");
    m_style = settings.value("style", "Fusion").toString();
    if(settings.value("custom_palette", false).toBool())
    {
        QString schemePath = settings.value("color_scheme_path").toString();
        m_customPalette = new QPalette(loadColorScheme(schemePath));
    }
    m_iconTheme = settings.value("icon_theme").toString();
    settings.endGroup();

    settings.beginGroup("Fonts");
    m_generalFont = settings.value("general", QPlatformTheme::font(QPlatformTheme::SystemFont)).value<QFont>();
    m_fixedFont = settings.value("fixed", QPlatformTheme::font(QPlatformTheme::FixedFont)).value<QFont>();
    settings.endGroup();

    settings.beginGroup("Interface");
    m_doubleClickInterval = QPlatformTheme::themeHint(QPlatformTheme::MouseDoubleClickInterval).toInt();
    m_doubleClickInterval = settings.value("double_click_interval", m_doubleClickInterval).toInt();
    m_cursorFlashTime = QPlatformTheme::themeHint(QPlatformTheme::CursorFlashTime).toInt();
    m_cursorFlashTime = settings.value("cursor_flash_time", m_cursorFlashTime).toInt();
    m_buttonBoxLayout = QPlatformTheme::themeHint(QPlatformTheme::DialogButtonBoxLayout).toInt();
    m_buttonBoxLayout = settings.value("buttonbox_layout", m_buttonBoxLayout).toInt();
    QCoreApplication::setAttribute(Qt::AA_DontShowIconsInMenus, !settings.value("menus_have_icons", true).toBool());

    //load effects
    m_uiEffects = QPlatformTheme::themeHint(QPlatformTheme::UiEffects).toInt();
    if(settings.childKeys().contains("gui_effects"))
    {
        QStringList effectList = settings.value("gui_effects").toStringList();
        m_uiEffects = 0;
        if(effectList.contains("General"))
            m_uiEffects |= QPlatformTheme::GeneralUiEffect;
        if(effectList.contains("AnimateMenu"))
            m_uiEffects |= QPlatformTheme::AnimateMenuUiEffect;
        if(effectList.contains("FadeMenu"))
            m_uiEffects |= QPlatformTheme::FadeMenuUiEffect;
        if(effectList.contains("AnimateCombo"))
            m_uiEffects |= QPlatformTheme::AnimateComboUiEffect;
        if(effectList.contains("AnimateTooltip"))
            m_uiEffects |= QPlatformTheme::AnimateTooltipUiEffect;
        if(effectList.contains("FadeTooltip"))
            m_uiEffects |= QPlatformTheme::FadeTooltipUiEffect;
        if(effectList.contains("AnimateToolBox"))
            m_uiEffects |= QPlatformTheme::AnimateToolBoxUiEffect;
    }

    //load style sheets
#ifdef QT_WIDGETS_LIB
    QStringList qssPaths = settings.value("stylesheets").toStringList();
    m_userStyleSheet = loadStyleSheets(qssPaths);
#endif
    settings.endGroup();
}

#ifdef QT_WIDGETS_LIB
bool Qt5CTPlatformTheme::hasWidgets()
{
    return qobject_cast<QApplication *> (qApp) != 0;
}
#endif

QString Qt5CTPlatformTheme::loadStyleSheets(const QStringList &paths)
{
    QString content;
    foreach (QString path, paths)
    {
        if(!QFile::exists(path))
            continue;

        QFile file(path);
        file.open(QIODevice::ReadOnly);
        content.append(file.readAll());
    }
    return content;
}

QPalette Qt5CTPlatformTheme::loadColorScheme(const QString &filePath)
{
    QPalette customPalette;
    QSettings settings(filePath, QSettings::IniFormat);
    settings.beginGroup("ColorScheme");
    QStringList activeColors = settings.value("active_colors").toStringList();
    QStringList inactiveColors = settings.value("inactive_colors").toStringList();
    QStringList disabledColors = settings.value("disabled_colors").toStringList();
    settings.endGroup();

    if(activeColors.count() == QPalette::NColorRoles &&
            inactiveColors.count() == QPalette::NColorRoles &&
            disabledColors.count() == QPalette::NColorRoles)
    {
        for (int i = 0; i < QPalette::NColorRoles; i++)
        {
            QPalette::ColorRole role = QPalette::ColorRole(i);
            customPalette.setColor(QPalette::Active, role, QColor(activeColors.at(i)));
            customPalette.setColor(QPalette::Inactive, role, QColor(inactiveColors.at(i)));
            customPalette.setColor(QPalette::Disabled, role, QColor(disabledColors.at(i)));
        }
    }
    else
    {
        customPalette = *QPlatformTheme::palette(SystemPalette); //load fallback palette
    }

    return customPalette;
}
