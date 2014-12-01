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

#ifndef QT5CTPLATFORMTHEME_H
#define QT5CTPLATFORMTHEME_H

#include <qpa/qplatformtheme.h>
#include <QFont>

class QPalette;

class Qt5CTPlatformTheme : public QPlatformTheme
{
public:
    Qt5CTPlatformTheme();

    virtual ~Qt5CTPlatformTheme();


    //virtual QPlatformMenuItem* createPlatformMenuItem() const;
    //virtual QPlatformMenu* createPlatformMenu() const;
    //virtual QPlatformMenuBar* createPlatformMenuBar() const;
    //virtual void showPlatformMenuBar() {}
    //virtual bool usePlatformNativeDialog(DialogType type) const;
    //virtual QPlatformDialogHelper *createPlatformDialogHelper(DialogType type) const;
    virtual const QPalette *palette(Palette type = SystemPalette) const;
    virtual const QFont *font(Font type = SystemFont) const;
    virtual QVariant themeHint(ThemeHint hint) const;
    //virtual QPixmap standardPixmap(StandardPixmap sp, const QSizeF &size) const;
    //virtual QPixmap fileIconPixmap(const QFileInfo &fileInfo, const QSizeF &size,
    //                               QPlatformTheme::IconOptions iconOptions = 0) const;

    //virtual QIconEngine *createIconEngine(const QString &iconName) const;
    //virtual QList<QKeySequence> keyBindings(QKeySequence::StandardKey key) const;
    //virtual QString standardButtonText(int button) const;

private:
    QString m_style, m_iconTheme;
    QPalette *m_customPalette;
    QFont m_generalFont, m_fixedFont;
};

#endif // QT5CTPLATFORMTHEME_H
