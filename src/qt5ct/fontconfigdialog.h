#ifndef FONTCONFIGDIALOG_H
#define FONTCONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class FontConfigDialog;
}

class FontConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FontConfigDialog(QWidget *parent = 0);
    ~FontConfigDialog();

public slots:
    void accept();

private:
    Ui::FontConfigDialog *m_ui;
};

#endif // FONTCONFIGDIALOG_H
