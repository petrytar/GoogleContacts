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

#include <QDesktopWidget>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) :
    BaseClass(parent),
    ui(new Ui::MainWindow),
    m_authManager(new data::AuthManager(this)),
    m_database(new data::Database(this)),
    m_googleContacts(new data::GoogleContacts(m_database, this))
{
    ui->setupUi(this);
    adjustUi();
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
        SelectUserDialog* selectUserDialog = new SelectUserDialog(users, this);
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
    VERIFY(connect(m_authManager, SIGNAL(newUserInitialiazed(data::ptr<data::User>)), this, SLOT(onNewUserInitialized(data::ptr<data::User>))));
    VERIFY(connect(m_authManager, SIGNAL(error()), this, SLOT(onAuthFailed())));
    m_loginDialog->show();
}

void MainWindow::setActiveUser(data::ptr<data::User> user)
{
    m_googleContacts->setActiveUser(user);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_googleContacts->loadContacts();
    VERIFY(connect(m_googleContacts, SIGNAL(contactsLoad()), this, SLOT(onContactsLoad())));
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

void MainWindow::onContactsLoad()
{
    updateWidgetsData();
    QApplication::restoreOverrideCursor();
}

void MainWindow::onAuthFailed()
{
    m_loginDialog->close();
    QMessageBox::critical(this, QString("Error"), QString("Failed authentication!"));
    exit(-1);
}

void MainWindow::updateWidgetsData()
{
//    ComboBoxDelegate* comboBoxDelegate = new ComboBoxDelegate(names, parent());
//    ui->tableView->setItemDelegateForColumn(1, comboBoxDelegate);

//    for (int row = 0; row < 4; ++row)
//    {
//        for (int column = 0; column < 2; ++column)
//        {
//            QModelIndex index = model->index(row, column, QModelIndex());
//            int value = (row+1) * (column+1);
//            std::cout << "Setting (" << row << ", " << column << ") to " << value << std::endl;
//            model->setData(index, QVariant(value));
//        }
//    }

//    // Make the combo boxes always displayed.
//    for ( int i = 0; i < model->rowCount(); ++i )
//    {
//        ui->tableView->openPersistentEditor( model->index(i, 1) );
//    }
    ui->entriesTreeWidget->clear();
    QList<data::ptr<data::ContactEntry>> contacts = m_googleContacts->getContacts();
    for (int row = 0; row < contacts.size(); ++row)
    {
        data::ptr<data::ContactEntry> contactEntry = contacts.at(row);
        QString nameToDisplay = contactEntry->getVisibleName();
        if (!nameToDisplay.isEmpty())
        {
            new QTreeWidgetItem(ui->entriesTreeWidget, QStringList() << nameToDisplay << contactEntry->getPrimaryEmail() << contactEntry->getPrimaryPhoneNumber());
        }
    }
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
    m_googleContacts->loadContacts();
    updateWidgetsData();
}
