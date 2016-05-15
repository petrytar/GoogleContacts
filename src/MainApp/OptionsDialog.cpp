#include "OptionsDialog.h"
#include "ui_OptionsDialog.h"

#include "MainApp/Settings.h"

OptionsDialog::OptionsDialog(Settings* settings, QWidget *parent) :
    BaseClass(parent),
    ui(new Ui::OptionsDialog),
    m_settings(settings)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    ui->newContactEdit->setText(QKeySequence(m_settings->getValue(Settings::E_SHORTCUT_NEW_CONTACT)).toString());
    ui->editContactEdit->setText(QKeySequence(m_settings->getValue(Settings::E_SHORTCUT_EDIT_CONTACT)).toString());
    ui->deleteContactEdit->setText(QKeySequence(m_settings->getValue(Settings::E_SHORTCUT_DELETE_CONTACT)).toString());
    ui->syncEdit->setText(QKeySequence(m_settings->getValue(Settings::E_SHORTCUT_SYNCHRONIZE)).toString());
    ui->exitEdit->setText(QKeySequence(m_settings->getValue(Settings::E_SHORTCUT_EXIT)).toString());
    ui->optionsEdit->setText(QKeySequence(m_settings->getValue(Settings::E_SHORTCUT_OPTIONS)).toString());
    ui->exportEdit->setText(QKeySequence(m_settings->getValue(Settings::E_SHORTCUT_EXPORT)).toString());
    ui->importEdit->setText(QKeySequence(m_settings->getValue(Settings::E_SHORTCUT_IMPORT)).toString());
    ui->syncIntervalSpinBox->setValue(m_settings->getValue(Settings::E_SYNCHRONIZATION_INTERVAL).toInt());
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

void OptionsDialog::accept()
{
    m_settings->setValue(Settings::E_SHORTCUT_NEW_CONTACT, QKeySequence(ui->newContactEdit->text()).toString());
    m_settings->setValue(Settings::E_SHORTCUT_EDIT_CONTACT, QKeySequence(ui->editContactEdit->text()).toString());
    m_settings->setValue(Settings::E_SHORTCUT_DELETE_CONTACT, QKeySequence(ui->deleteContactEdit->text()).toString());
    m_settings->setValue(Settings::E_SHORTCUT_SYNCHRONIZE, QKeySequence(ui->syncEdit->text()).toString());
    m_settings->setValue(Settings::E_SHORTCUT_EXIT, QKeySequence(ui->exitEdit->text()).toString());
    m_settings->setValue(Settings::E_SHORTCUT_OPTIONS, QKeySequence(ui->optionsEdit->text()).toString());
    m_settings->setValue(Settings::E_SHORTCUT_EXPORT, QKeySequence(ui->exportEdit->text()).toString());
    m_settings->setValue(Settings::E_SHORTCUT_IMPORT, QKeySequence(ui->importEdit->text()).toString());
    m_settings->setValue(Settings::E_SYNCHRONIZATION_INTERVAL, QString::number(ui->syncIntervalSpinBox->value()));
    BaseClass::accept();
}

void OptionsDialog::on_resetToDefaultButton_clicked()
{
    ui->newContactEdit->setText(QKeySequence(m_settings->getDefaultValue(Settings::E_SHORTCUT_NEW_CONTACT)).toString());
    ui->editContactEdit->setText(QKeySequence(m_settings->getDefaultValue(Settings::E_SHORTCUT_EDIT_CONTACT)).toString());
    ui->deleteContactEdit->setText(QKeySequence(m_settings->getDefaultValue(Settings::E_SHORTCUT_DELETE_CONTACT)).toString());
    ui->syncEdit->setText(QKeySequence(m_settings->getDefaultValue(Settings::E_SHORTCUT_SYNCHRONIZE)).toString());
    ui->exitEdit->setText(QKeySequence(m_settings->getDefaultValue(Settings::E_SHORTCUT_EXIT)).toString());
    ui->optionsEdit->setText(QKeySequence(m_settings->getDefaultValue(Settings::E_SHORTCUT_OPTIONS)).toString());
    ui->exportEdit->setText(QKeySequence(m_settings->getDefaultValue(Settings::E_SHORTCUT_EXPORT)).toString());
    ui->importEdit->setText(QKeySequence(m_settings->getDefaultValue(Settings::E_SHORTCUT_IMPORT)).toString());
    ui->syncIntervalSpinBox->setValue(m_settings->getDefaultValue(Settings::E_SYNCHRONIZATION_INTERVAL).toInt());
}

void OptionsDialog::on_resetNewContactButton_clicked()
{
    ui->newContactEdit->setText("");
}

void OptionsDialog::on_resetExitButton_clicked()
{
    ui->exitEdit->setText("");
}

void OptionsDialog::on_resetEditContactButton_clicked()
{
    ui->editContactEdit->setText("");
}

void OptionsDialog::on_resetDeleteContactButton_clicked()
{
    ui->deleteContactEdit->setText("");
}

void OptionsDialog::on_resetSyncButton_clicked()
{
    ui->syncEdit->setText("");
}

void OptionsDialog::on_resetOptionsButton_clicked()
{
    ui->optionsEdit->setText("");
}

void OptionsDialog::on_resetExportButton_clicked()
{
    ui->exportEdit->setText("");
}

void OptionsDialog::on_resetImportButton_clicked()
{
    ui->importEdit->setText("");
}
