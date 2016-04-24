#ifndef MAINAPP_MAINWINDOW_H
#define MAINAPP_MAINWINDOW_H

#include <QMainWindow>

#include "MainApp/MainApp_global.h"

class QNetworkAccessManager;
class QTreeWidgetItem;

namespace data
{
class AuthManager;
class GoogleContacts;
class Database;
class User;
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
    void onNewUserInitialized(data::ptr<data::User> user);

    void onAuthFailed();
    void onLoginLoadFailed();

    void onContactsLoad();
    void onContactsLoadFailed();

    void initNewUser();
    void setActiveUser(data::ptr<data::User> user);

    void on_syncButton_clicked();
    void on_editButton_clicked();
    void on_entriesTreeWidget_itemSelectionChanged();
    void on_newButton_clicked();
    void on_deleteButton_clicked();

private:
    // helping enum for tableWidget columns
    enum EColumn
    {
        E_COLUMN__BEGIN = 0,
        E_COLUMN_NAME_TO_DISPLAY = E_COLUMN__BEGIN,
        E_COLUMN_EMAILS,
        E_COLUMN_PHONE_NUMBERS,
        E_COLUMN__END
    };

    /// Customize the appearance of dialogue
    void adjustUi();

    bool isAccessTokenEnabled() const;
    void fillContactEntriesTreeWidget();
    void updateContactEntryItem(QTreeWidgetItem* item);

    void setUserContactSingleValueRows(int column, const QStringList& userContactNames);
    void setUserContactListValueRows(int column, const QList<QStringList>& phoneNumbersList);

    Ui::MainWindow* ui;
    LoginDialog* m_loginDialog;

    QNetworkAccessManager* m_networkAccessManager;
    data::AuthManager* m_authManager;
    data::Database* m_database;
    data::GoogleContacts* m_googleContacts;
};

#endif // MAINAPP_MAINWINDOW_H
