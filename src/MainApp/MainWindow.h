#ifndef MAINAPP_MAINWINDOW_H
#define MAINAPP_MAINWINDOW_H

#include "MainApp/LoginDialog.h"

#include <QMainWindow>

namespace data
{
class AuthManager;
class GoogleContacts;
}

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    typedef QMainWindow BaseClass;

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

    void setup();

private slots:
    void onAuthSuccessful();
    void onAuthFailed();
    void onLoginLoadFailed();

private:
    /// Customize the appearance of dialogue
    void adjustUi();
    bool isAccessTokenEnabled() const;

    Ui::MainWindow* ui;
    LoginDialog* m_loginDialog;

    data::AuthManager* m_authManager;
    data::GoogleContacts* m_googleContacts;
};

#endif // MAINAPP_MAINWINDOW_H
