#include "fontconfigdialog.h"
#include "ui_fontconfigdialog.h"

FontConfigDialog::FontConfigDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::FontConfigDialog)
{
    m_ui->setupUi(this);

    m_ui->hintingStyleComboBox->addItem(tr("None"), "hintnone");
    m_ui->hintingStyleComboBox->addItem(tr("Slight"), "hintslight");
    m_ui->hintingStyleComboBox->addItem(tr("Medium"), "hintmedium");
    m_ui->hintingStyleComboBox->addItem(tr("Full"), "hintfull");

    m_ui->rgbaComboBox->addItem(tr("None"), "none");
    m_ui->rgbaComboBox->addItem("rgb", "rgb");
    m_ui->rgbaComboBox->addItem("bgr", "bgr");
    m_ui->rgbaComboBox->addItem("vrgb", "vrgb");
    m_ui->rgbaComboBox->addItem("vbgr", "vbgr");

    m_ui->lcdFilterComboBox->addItem("lcdnone");
    m_ui->lcdFilterComboBox->addItem("lcddefault");
    m_ui->lcdFilterComboBox->addItem("lcdlight");
    m_ui->lcdFilterComboBox->addItem("lcdlegacy");
}

FontConfigDialog::~FontConfigDialog()
{
    delete m_ui;
}

void FontConfigDialog::accept()
{
    QDialog::accept();
}
