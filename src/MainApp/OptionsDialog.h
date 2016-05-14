#ifndef MAINAPP_OPTIONSDIALOG_H
#define MAINAPP_OPTIONSDIALOG_H

#include <QDialog>

#include "MainApp/MainApp_global.h"

namespace Ui
{
class OptionsDialog;
}

class Settings;

class OptionsDialog : public QDialog
{
    Q_OBJECT
private:
    typedef QDialog BaseClass;

public:
    explicit OptionsDialog(Settings* settings, QWidget* parent);
    ~OptionsDialog();

    virtual void accept() override;

private slots:
    void on_resetNewContactButton_clicked();
    void on_resetExitButton_clicked();
    void on_resetEditContactButton_clicked();
    void on_resetDeleteContactButton_clicked();
    void on_resetSyncButton_clicked();
    void on_resetOptionsButton_clicked();
    void on_resetToDefaultButton_clicked();

private:
    Ui::OptionsDialog* ui;
    Settings* m_settings;
};

#endif // MAINAPP_OPTIONSDIALOG_H
