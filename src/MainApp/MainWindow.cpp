#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Data/Auth/AuthManager.h"
#include "Data/Database.h"
#include "Data/GoogleContacts.h"
#include "Data/debugAsserts.h"
#include "Data/Model/ContactEntry.h"
#include "Data/Model/User.h"
#include "MainApp/ComboBoxDelegate.h"
#include "MainApp/LoginDialog.h"
#include "MainApp/SelectUserDialog.h"
#include "MainApp/EditContactEntryDialog.h"

#include <QDesktopWidget>
#include <QMessageBox>
#include <QNetworkAccessManager>

MainWindow::MainWindow(QWidget* parent) :
    BaseClass(parent),
    ui(new Ui::MainWindow),
    m_networkAccessManager(new QNetworkAccessManager(this)),
    m_authManager(new data::AuthManager(m_networkAccessManager, this)),
    m_database(new data::Database(this)),
    m_googleContacts(new data::GoogleContacts(m_networkAccessManager, m_database, this))
{
    ui->setupUi(this);
    adjustUi();

    VERIFY(connect(m_authManager, SIGNAL(newUserInitialiazed(data::ptr<data::User>)), this, SLOT(onNewUserInitialized(data::ptr<data::User>))));
    VERIFY(connect(m_authManager, SIGNAL(error()), this, SLOT(onAuthFailed())));
    VERIFY(connect(m_authManager, SIGNAL(accessTokenReceived(QString)), this, SLOT(onAccessTokenReceived(QString))));
    VERIFY(connect(m_authManager, SIGNAL(invalidRefreshToken()), this, SLOT(onInvalidRefreshToken())));
    VERIFY(connect(m_googleContacts, SIGNAL(contactsSyncSuccessful()), this, SLOT(onContactsSyncSuccessful())));
    VERIFY(connect(m_googleContacts, SIGNAL(authorizationError()), this, SLOT(onContactsAuthorizationError())));
    VERIFY(connect(m_googleContacts, SIGNAL(otherError(QNetworkReply::NetworkError)), this, SLOT(onContactsOtherError(QNetworkReply::NetworkError))));
}

bool MainWindow::isAccessTokenEnabled() const
{
    return m_authManager->getAccessToken().isEmpty() ? false : true;
}

void MainWindow::adjustUi()
{
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));

    ui->entriesTreeWidget->setHeaderLabels(QStringList() << "Name" << "Email" << "Phone");
    ui->entriesTreeWidget->setColumnWidth(E_COLUMN_NAME_TO_DISPLAY, 200);
    ui->entriesTreeWidget->setColumnWidth(E_COLUMN_EMAILS, 150);
    ui->entriesTreeWidget->setIndentation(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setup()
{
    m_database->open();

    QList<data::ptr<data::User>> users = m_database->getUsers();
    if (users.empty())
    {
        initNewUser();
    }
    else
    {
        SelectUserDialog* selectUserDialog = new SelectUserDialog(m_database, users, parentWidget());
        VERIFY(connect(selectUserDialog, SIGNAL(newUserRequested()), this, SLOT(initNewUser())));
        VERIFY(connect(selectUserDialog, SIGNAL(userSelected(data::ptr<data::User>)), this, SLOT(setActiveUser(data::ptr<data::User>))));
        selectUserDialog->open();
        qDebug() << "show selectUserDialog";
    }
}

void MainWindow::initNewUser()
{
    m_authManager->start();
    m_loginDialog = new LoginDialog(m_authManager->generateAuthorizationRequestUrl(), parentWidget());
    VERIFY(connect(m_loginDialog, SIGNAL(loadFailed()), this, SLOT(onLoginLoadFailed())));

    m_loginDialog->show();
}

void MainWindow::setActiveUser(data::ptr<data::User> user)
{
    m_googleContacts->setActiveUser(user);
    fillContactEntriesTreeWidget();
    m_googleContacts->syncContacts();
    show();
}

void MainWindow::onNewUserInitialized(data::ptr<data::User> user)
{
    if (m_loginDialog)
    {
        m_loginDialog->close();
    }

    m_database->saveOrGetByEmail(user);
    setActiveUser(user);
}

void MainWindow::onLoginLoadFailed()
{
    m_loginDialog->close();
    QMessageBox::critical(this, QString("Error"), QString("Failed to login!"));
    exit(-1);
}

void MainWindow::onAuthFailed()
{
    m_loginDialog->close();
    QMessageBox::critical(this, QString("Error"), QString("Failed authentication!"));
    exit(-1);
}

void MainWindow::onContactsSyncSuccessful()
{
    fillContactEntriesTreeWidget();
    ui->statusBar->showMessage(QString("Successfully syncronized on %1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")));
}

void MainWindow::onContactsAuthorizationError()
{
    ui->statusBar->showMessage(QString("Failed to syncronize on %1 (authorization error)").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")));
    m_authManager->refreshAccessToken(m_googleContacts->getActiveUser()->getRefreshToken());
}

void MainWindow::onContactsOtherError(QNetworkReply::NetworkError error)
{
    fillContactEntriesTreeWidget();
    ui->statusBar->showMessage(QString("Failed to syncronize on %1 (error code: %2)").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")).arg(error));
}

void MainWindow::onAccessTokenReceived(const QString& accessToken)
{
    m_googleContacts->getActiveUser()->setAccessToken(accessToken);
    m_database->save(m_googleContacts->getActiveUser());
    m_googleContacts->syncContacts();
}

void MainWindow::onInvalidRefreshToken()
{
    ui->statusBar->showMessage("Failed to refresh access token - invalid refresh token");
}

void MainWindow::fillContactEntriesTreeWidget()
{
    ui->entriesTreeWidget->clear();
    QList<data::ptr<data::ContactEntry>> contacts = m_googleContacts->getContacts();
    for (int row = 0; row < contacts.size(); ++row)
    {
        data::ptr<data::ContactEntry> contactEntry = contacts.at(row);
        if (contactEntry->isDeleted())
        {
            continue;
        }

        QTreeWidgetItem* item = new QTreeWidgetItem(ui->entriesTreeWidget);
        item->setData(0, Qt::UserRole, QVariant::fromValue(contactEntry));
        updateContactEntryItem(item);
    }
}

void MainWindow::updateContactEntryItem(QTreeWidgetItem* item)
{
    data::ptr<data::ContactEntry> contactEntry = item->data(0, Qt::UserRole).value<data::ptr<data::ContactEntry>>();
    item->setText(0, contactEntry->getVisibleName());
    item->setText(1, contactEntry->getPrimaryEmail());
    item->setText(2, contactEntry->getPrimaryPhoneNumber());
}

void MainWindow::setUserContactSingleValueRows(int column, const QStringList& userContactNames)
{
    for (int i = 0; i < userContactNames.size(); ++i)
    {
        //        ui->tableView->setItem(i, column, new QTableWidgetItem(userContactNames.at(i)));
    }
}

void MainWindow::setUserContactListValueRows(int column, const QList<QStringList>& phoneNumbersList)
{
    for (int i = 0; i < phoneNumbersList.size(); ++i)
    {
        for (int j = 0; j < phoneNumbersList.at(i).size(); ++j)
        {
            //            ui->tableView->setItem(i, column, new QTableWidgetItem(userContactNames.at(j)));
        }
    }
}

void MainWindow::on_syncButton_clicked()
{
    m_googleContacts->syncContacts();
    //fillContactEntriesTreeWidget();
}

void MainWindow::on_editButton_clicked()
{
    QTreeWidgetItem* selectedItem = ui->entriesTreeWidget->selectedItems().at(0);
    data::ptr<data::ContactEntry> contactEntry = selectedItem->data(0, Qt::UserRole).value<data::ptr<data::ContactEntry>>();
    EditContactEntryDialog* editContactEntryDialog = new EditContactEntryDialog(contactEntry, this);
    auto onAccepted = [this, selectedItem, contactEntry]()
    {
        m_database->update(contactEntry);
        updateContactEntryItem(selectedItem);
    };
    VERIFY(connect(editContactEntryDialog, &QDialog::accepted, onAccepted));
    editContactEntryDialog->open();
}

void MainWindow::on_entriesTreeWidget_itemSelectionChanged()
{
    bool hasSelectedItem = !ui->entriesTreeWidget->selectedItems().empty();
    ui->editButton->setEnabled(hasSelectedItem);
    ui->deleteButton->setEnabled(hasSelectedItem);
}

void MainWindow::on_newButton_clicked()
{
    data::ptr<data::ContactEntry> newContactEntry(new data::ContactEntry());
    EditContactEntryDialog* editContactEntryDialog = new EditContactEntryDialog(newContactEntry, this);
    auto onAccepted = [this, newContactEntry]()
    {
        m_googleContacts->addContact(newContactEntry);
        fillContactEntriesTreeWidget();
    };
    VERIFY(connect(editContactEntryDialog, &QDialog::accepted, onAccepted));
    editContactEntryDialog->open();
}

void MainWindow::on_deleteButton_clicked()
{
    QTreeWidgetItem* selectedItem = ui->entriesTreeWidget->selectedItems().at(0);
        int result = QMessageBox::question(this, "Delete contact", QString("Do you want to delete the selected contact entry?"),
            QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
    if (result == QMessageBox::Ok)
    {
        data::ptr<data::ContactEntry> contactEntry = selectedItem->data(0, Qt::UserRole).value<data::ptr<data::ContactEntry>>();
        contactEntry->setDeleted(true);
        m_database->save(contactEntry);
        fillContactEntriesTreeWidget();
    }
}
