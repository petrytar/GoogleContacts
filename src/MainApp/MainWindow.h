#ifndef MAINAPP_MAINWINDOW_H
#define MAINAPP_MAINWINDOW_H

#include <QMainWindow>

namespace data
{
class AuthManager;
class GoogleContacts;
class Database;
}

class LoginDialog;

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
    void onContactsLoad();

    void on_syncButton_clicked();

private:
    // helping enum for tableWidget columns
    enum EColumn
    {
        E_COLUMN__BEGIN = 0,
        E_COLUMN_NAME_TO_DISPLAY = E_COLUMN__BEGIN,
        E_COLUMN_EMAILS,
        E_COLUMN_PHONE_NUMBERS,
        /*E_COLUMN_JOB,
        E_COLUMN_COMPANY,*/
        E_COLUMN__END
    };

    /// Customize the appearance of dialogue
    void adjustUi();
    bool isAccessTokenEnabled() const;
    void updateWidgetsData();
    void setUserContactSingleValueRows(int column, const QStringList& userContactNames);
    void setUserContactListValueRows(int column, const QList<QStringList>& phoneNumbersList);

    Ui::MainWindow* ui;
    LoginDialog* m_loginDialog;

    data::AuthManager* m_authManager;
    data::Database* m_database;
    data::GoogleContacts* m_googleContacts;
};

#endif // MAINAPP_MAINWINDOW_H
