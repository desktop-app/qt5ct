/*
 * Copyright (c) 2014-2019, Ilya Kotov <forkotov02@ya.ru>
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
#include <QTimer>
#include <QIcon>
#include <QRegExp>
#include <QMimeDatabase>
#ifdef QT_WIDGETS_LIB
#include <QStyle>
#include <QStyleFactory>
#include <QApplication>
#include <QWidget>
#endif
#include <QFile>
#include <QFileSystemWatcher>

#include <qt5ct/qt5ct.h>
#include "qt5ctplatformtheme.h"
#ifdef GLOBAL_MENU
#include <private/qdbusmenubar_p.h>
#endif
#ifdef DBUS_TRAY
#include <private/qdbustrayicon_p.h>
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
#include <QStringList>
#include <qpa/qplatformthemefactory_p.h>
#endif

Q_LOGGING_CATEGORY(lqt5ct, "qt5ct")

//QT_QPA_PLATFORMTHEME=qt5ct

Qt5CTPlatformTheme::Qt5CTPlatformTheme()
{
    Qt5CT::initConfig();
    if(QGuiApplication::desktopSettingsAware())
    {
        readSettings();
        QMetaObject::invokeMethod(this, "applySettings", Qt::QueuedConnection);
#ifdef QT_WIDGETS_LIB
        QMetaObject::invokeMethod(this, "createFSWatcher", Qt::QueuedConnection);
#endif
        QGuiApplication::setFont(m_generalFont);
    }
    qCDebug(lqt5ct) << "using qt5ct plugin";
#ifdef QT_WIDGETS_LIB
    if(!QStyleFactory::keys().contains("qt5ct-style"))
        qCCritical(lqt5ct) << "unable to find qt5ct proxy style";
#endif
}

Qt5CTPlatformTheme::~Qt5CTPlatformTheme()
{
    if(m_customPalette)
        delete m_customPalette;

}

#ifdef GLOBAL_MENU
QPlatformMenuBar *Qt5CTPlatformTheme::createPlatformMenuBar() const
{
    if(m_checkDBusGlobalMenu)
    {
        QDBusConnection conn = QDBusConnection::sessionBus();
        m_dbusGlobalMenuAvailable = conn.interface()->isServiceRegistered("com.canonical.AppMenu.Registrar");
        m_checkDBusGlobalMenu = false;
        qCDebug(lqt5ct) << "D-Bus global menu:" << (m_dbusGlobalMenuAvailable ? "yes" : "no");
    }
    return (m_dbusGlobalMenuAvailable ? new QDBusMenuBar() : nullptr);
}
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
bool Qt5CTPlatformTheme::usePlatformNativeDialog(DialogType type) const
{
    return m_theme ? m_theme->usePlatformNativeDialog(type) :
                     QPlatformTheme::usePlatformNativeDialog(type);
}

QPlatformDialogHelper *Qt5CTPlatformTheme::createPlatformDialogHelper(DialogType type) const
{
    return m_theme ? m_theme->createPlatformDialogHelper(type) :
                     QPlatformTheme::createPlatformDialogHelper(type);
}
#endif

#ifdef DBUS_TRAY
QPlatformSystemTrayIcon *Qt5CTPlatformTheme::createPlatformSystemTrayIcon() const
{
    if(m_checkDBusTray)
    {
        QDBusMenuConnection conn;
        m_dbusTrayAvailable = conn.isStatusNotifierHostRegistered();
        m_checkDBusTray = false;
        qCDebug(lqt5ct) << "D-Bus system tray:" << (m_dbusTrayAvailable ? "yes" : "no");
    }
    return (m_dbusTrayAvailable ? new QDBusTrayIcon() : nullptr);
}
#endif

const QPalette *Qt5CTPlatformTheme::palette(QPlatformTheme::Palette type) const
{
    Q_UNUSED(type);
    return (m_usePalette ? m_customPalette : nullptr);
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
    case QPlatformTheme::ToolButtonStyle:
        return m_toolButtonStyle;
    case QPlatformTheme::SystemIconThemeName:
        return m_iconTheme;
    case QPlatformTheme::StyleNames:
        return QStringList() << "qt5ct-style";
    case QPlatformTheme::IconThemeSearchPaths:
        return Qt5CT::iconPaths();
    case QPlatformTheme::DialogButtonBoxLayout:
        return m_buttonBoxLayout;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
    case QPlatformTheme::KeyboardScheme:
        return m_keyboardScheme;
#endif
    case QPlatformTheme::UiEffects:
        return m_uiEffects;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    case QPlatformTheme::WheelScrollLines:
        return m_wheelScrollLines;
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    case QPlatformTheme::ShowShortcutsInContextMenus:
        return m_showShortcutsInContextMenus;
#endif
    default:
        return QPlatformTheme::themeHint(hint);
    }
}
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
QIcon Qt5CTPlatformTheme::fileIcon(const QFileInfo &fileInfo, QPlatformTheme::IconOptions iconOptions) const
{
    if((iconOptions & DontUseCustomDirectoryIcons) && fileInfo.isDir())
        return QIcon::fromTheme(QLatin1String("inode-directory"));

    QMimeDatabase db;
    QMimeType type = db.mimeTypeForFile(fileInfo);
    return QIcon::fromTheme(type.iconName());
}
#endif

void Qt5CTPlatformTheme::applySettings()
{
    if(!QGuiApplication::desktopSettingsAware())
        return;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
    if(!m_update)
    {
        //do not override application palette
        if(QCoreApplication::testAttribute(Qt::AA_SetPalette))
        {
            m_usePalette = false;
            qCDebug(lqt5ct) << "palette support is disabled";
        }
    }
#endif

#ifdef QT_WIDGETS_LIB
    if(hasWidgets())
    {
        qApp->setFont(m_generalFont);

        //Qt 5.6 or higher should be use themeHint function on application startup.
        //So, there is no need to call this function first time.
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
        if(m_update)
            qApp->setWheelScrollLines(m_wheelScrollLines);
#else
        qApp->setWheelScrollLines(m_wheelScrollLines);
#endif

        if(m_update && qApp->style()->objectName() == "qt5ct-style") //ignore application style
            qApp->setStyle("qt5ct-style"); //recreate style object

        if(m_update && m_usePalette)
        {
            if(m_customPalette)
                qApp->setPalette(*m_customPalette);
            else
                qApp->setPalette(qApp->style()->standardPalette());
        }

        //do not override application style
        if(m_prevStyleSheet == qApp->styleSheet())
            qApp->setStyleSheet(m_userStyleSheet);
        else
            qCDebug(lqt5ct) << "custom style sheet is disabled";
        m_prevStyleSheet = m_userStyleSheet;
    }
#endif
    QGuiApplication::setFont(m_generalFont); //apply font
    QIcon::setThemeName(m_iconTheme); //apply icons
    if(m_customPalette && m_usePalette)
        QGuiApplication::setPalette(*m_customPalette); //apply palette

#ifdef QT_WIDGETS_LIB
    if(m_customPalette && m_usePalette && !m_update)
        qApp->setPalette(*m_customPalette);

    if(hasWidgets())
    {
        for(QWidget *w : qApp->allWidgets())
        {
            QEvent e(QEvent::ThemeChange);
            QApplication::sendEvent(w, &e);
        }
    }
#endif

    if(!m_update)
        m_update = true;
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
    qCDebug(lqt5ct) << "updating settings..";
    readSettings();
    applySettings();
}
#endif

void Qt5CTPlatformTheme::readSettings()
{
    if(m_customPalette)
    {
        delete m_customPalette;
        m_customPalette = nullptr;
    }

    QSettings settings(Qt5CT::configFile(), QSettings::IniFormat);

    settings.beginGroup("Appearance");
    m_style = settings.value("style", "Fusion").toString();
    QString schemePath = settings.value("color_scheme_path").toString();
    if(!schemePath.isEmpty() && settings.value("custom_palette", false).toBool())
    {
        schemePath = Qt5CT::resolvePath(schemePath); //replace environment variables
        m_customPalette = new QPalette(loadColorScheme(schemePath));
    }
    m_iconTheme = settings.value("icon_theme").toString();
    //load dialogs
#if (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
    if(!m_update)
    {
        //do not mix gtk2 style and gtk3 dialogs
        QStringList keys = QPlatformThemeFactory::keys();
        QString name = settings.value("standard_dialogs", "default").toString();
        if((m_style == QLatin1String("gtk2") || m_style == QLatin1String("qt5gtk2")) &&
                (name == QLatin1String("gtk3") || name == QLatin1String("qt5gtk3")))
        {
            name = QLatin1String("gtk2");
        }
        if(keys.contains(name))
            m_theme.reset(QPlatformThemeFactory::create(name));
        else if(name == QLatin1String("gtk2") && keys.contains("qt5gtk2"))
            m_theme.reset(QPlatformThemeFactory::create("qt5gtk2"));
        else if(name == QLatin1String("gtk3") && keys.contains("qt5gtk3"))
            m_theme.reset(QPlatformThemeFactory::create("qt5gtk3"));
    }
#endif

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
    m_showShortcutsInContextMenus = settings.value("show_shortcuts_in_context_menus", true).toBool();
    m_buttonBoxLayout = QPlatformTheme::themeHint(QPlatformTheme::DialogButtonBoxLayout).toInt();
    m_buttonBoxLayout = settings.value("buttonbox_layout", m_buttonBoxLayout).toInt();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
    m_keyboardScheme = QPlatformTheme::themeHint(QPlatformTheme::KeyboardScheme).toInt();
    m_keyboardScheme = settings.value("keyboard_scheme", m_keyboardScheme).toInt();
#endif
    QCoreApplication::setAttribute(Qt::AA_DontShowIconsInMenus, !settings.value("menus_have_icons", true).toBool());
    m_toolButtonStyle = settings.value("toolbutton_style", Qt::ToolButtonFollowStyle).toInt();
    m_wheelScrollLines = settings.value("wheel_scroll_lines", 3).toInt();

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
    return qobject_cast<QApplication *> (qApp) != nullptr;
}
#endif

QString Qt5CTPlatformTheme::loadStyleSheets(const QStringList &paths)
{
    QString content;
    for(const QString &path : paths)
    {
        if(!QFile::exists(path))
            continue;

        QFile file(path);
        file.open(QIODevice::ReadOnly);
        content.append(QString::fromUtf8(file.readAll()));
    }
    QRegExp regExp("//.*(\\n|$)");
    regExp.setMinimal(true);
    content.remove(regExp);
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
#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
    else if(activeColors.count() == QPalette::NColorRoles - 1 &&
            inactiveColors.count() == QPalette::NColorRoles - 1 &&
            disabledColors.count() == QPalette::NColorRoles - 1)
    {
        //old format compatibility
        for (int i = 0; i < QPalette::NColorRoles - 1; i++)
        {
            QPalette::ColorRole role = QPalette::ColorRole(i);
            customPalette.setColor(QPalette::Active, role, QColor(activeColors.at(i)));
            customPalette.setColor(QPalette::Inactive, role, QColor(inactiveColors.at(i)));
            customPalette.setColor(QPalette::Disabled, role, QColor(disabledColors.at(i)));
        }
        QColor textColor = customPalette.text().color();
        textColor.setAlpha(128);
        customPalette.setColor(QPalette::Active, QPalette::PlaceholderText, textColor);
        customPalette.setColor(QPalette::Inactive, QPalette::PlaceholderText, textColor);
        customPalette.setColor(QPalette::Disabled, QPalette::PlaceholderText, textColor);
    }
#endif
    else
    {
        customPalette = *QPlatformTheme::palette(SystemPalette); //load fallback palette
    }

    return customPalette;
}
