#ifndef MAINAPP_MAINWINDOW_H
#define MAINAPP_MAINWINDOW_H

#include <QMainWindow>

#include "MainApp/MainApp_global.h"

#include <QNetworkReply>

class QNetworkAccessManager;
class QTreeWidgetItem;
class QRadioButton;
class QTimer;
class QLabel;

namespace data
{
class AuthManager;
class GoogleContacts;
class Database;
class User;
class ContactGroup;
}

class LoginDialog;
class Settings;

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

    void onAccessTokenReceived(const QString& accessToken);
    void onInvalidRefreshToken();

    void onGroupsSyncSuccessful();
    void onContactsSyncSuccessful();
    void onContactsAuthorizationError();
    void onContactsOtherError(QNetworkReply::NetworkError error);

    void initNewUser();
    void setActiveUser(data::ptr<data::User> user);

    void onGroupRadioButtonToggled();

    void onSyncTimerTimeout();

    void on_syncButton_clicked();
    void on_editButton_clicked();
    void on_entriesTreeWidget_itemSelectionChanged();
    void on_newButton_clicked();
    void on_deleteButton_clicked();
    void on_findLineEdit_textChanged();
    void on_actionNewContact_triggered();
    void on_actionEditContact_triggered();
    void on_actionDeleteContact_triggered();
    void on_actionSynchronize_triggered();
    void on_actionExit_triggered();
    void on_actionOptions_triggered();

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

    void applySettings();

    void createNewContact();
    void editContact();
    void deleteContact();
    void synchronizeContacts();
    void showOptionsDialog();

    void fillContactGroupsTreeWidget();
    void fillContactEntriesTreeWidget();
    void updateContactEntryItem(QTreeWidgetItem* item);
    void showFilteredContactEntryItems();
    QList<QTreeWidgetItem*> filterContactEntryItems(QList<QTreeWidgetItem*> contactEntryTreeItems);

    bool itemMatchesFindFilter(QTreeWidgetItem* item);

    void setUserContactSingleValueRows(int column, const QStringList& userContactNames);
    void setUserContactListValueRows(int column, const QList<QStringList>& phoneNumbersList);

    void updateStatusLabel(bool ok, const QString& message);

    Ui::MainWindow* ui;
    LoginDialog* m_loginDialog;
    QLabel* m_statusLabel;
    QLabel* m_statusIconLabel;

    Settings* m_settings;

    QNetworkAccessManager* m_networkAccessManager;
    data::AuthManager* m_authManager;
    data::Database* m_database;
    data::GoogleContacts* m_googleContacts;

    QTimer* m_syncTimer;

    QMap<QRadioButton*, data::ptr<data::ContactGroup>> m_groupRadioButtons;
    QList<QTreeWidgetItem*> m_allContactEntryTreeItems;
};

#endif // MAINAPP_MAINWINDOW_H
