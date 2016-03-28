#ifndef MAINAPP_LOGINDIALOG_H
#define MAINAPP_LOGINDIALOG_H

#include <QDialog>
#include <QUrl>

namespace Ui
{
    class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT
private:
    typedef QDialog BaseClass;

public:
    explicit LoginDialog(QUrl url, QWidget* parent = 0);
    ~LoginDialog();

private:
    Ui::LoginDialog* ui;
};

#endif // MAINAPP_LOGINDIALOG_H
