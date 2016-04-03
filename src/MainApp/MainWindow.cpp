#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Data/Auth/AuthManager.h"
#include "Data/GoogleContacts.h"
#include "Data/debugAsserts.h"
#include "MainApp/ComboBoxDelegate.h"

#include <QDesktopWidget>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) :
    BaseClass(parent),
    ui(new Ui::MainWindow),
    m_authManager(new data::AuthManager(this)),
    m_googleContacts(new data::GoogleContacts(this)),
    m_tableModel(nullptr)
{
    ui->setupUi(this);
    adjustUi();
}

bool MainWindow::isAccessTokenEnabled() const
{
    return false;
}

void MainWindow::adjustUi()
{
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setup()
{
    if (!isAccessTokenEnabled())
    {
        m_authManager->start();
        m_loginDialog = new LoginDialog(m_authManager->generateAuthorizationRequestUrl(), parentWidget());
        VERIFY(connect(m_loginDialog, SIGNAL(loadFailed()), this, SLOT(onLoginLoadFailed())));
        VERIFY(connect(m_authManager, SIGNAL(accessTokenReceived()), this, SLOT(onAuthSuccessful())));
        VERIFY(connect(m_authManager, SIGNAL(error()), this, SLOT(onAuthFailed())));
        m_loginDialog->show();
    }
    else
    {
        show();
    }
}

void MainWindow::onLoginLoadFailed()
{
    m_loginDialog->close();
    QMessageBox::critical(this, QString("Error"), QString("Failed to login!"));
    exit(-1);
}

void MainWindow::onContactsLoad()
{
    m_tableModel = new QStandardItemModel(0, E_COLUMN__END, parent());
    ui->tableView->setModel(m_tableModel);

    updateWidgetsData();
    QApplication::restoreOverrideCursor();
}

void MainWindow::onAuthSuccessful()
{
    m_loginDialog->close();
    m_googleContacts->setAccessToken(m_authManager->getAccessToken());
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_googleContacts->loadContacts();
    VERIFY(connect(m_googleContacts, SIGNAL(contactsLoad()), this, SLOT(onContactsLoad())));
    show();
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
    QList<data::ContactEntry*> contacts = m_googleContacts->getContacts();
    for (int row = 0; row < contacts.size(); ++row)
    {
        QString nameToDisplay = contacts.at(row)->getFileAs();
        if (nameToDisplay.isEmpty())
        {
            ContactPropertyPtr givenNameContactProperty = contacts.at(row)->getGivenName();
            if (givenNameContactProperty)
            {
                nameToDisplay += givenNameContactProperty->getValue();
            }

            ContactPropertyPtr familyNameContactProperty = contacts.at(row)->getFamilyName();
            if (familyNameContactProperty)
            {
                nameToDisplay += " " + familyNameContactProperty->getValue();
            }
        }

        if (!nameToDisplay.isEmpty())
        {
            m_tableModel->insertRow(m_tableModel->rowCount());
            QModelIndex index = m_tableModel->index(m_tableModel->rowCount() - 1, E_COLUMN_NAME_TO_DISPLAY, QModelIndex());
            qDebug() << nameToDisplay;
            m_tableModel->setData(index, nameToDisplay);
        }
    }
    // set names
//    setUserContactSingleValueRows(E_COLUMN_NAME, m_googleContacts->getContacts().);
    // set lists of emails
    QList<QStringList> phoneNumbers_0;
    phoneNumbers_0.push_back(QStringList() << "+420 777 777 770" << "+420 777 777 771" << "+420 777 777 772");
    QList<QStringList> phoneNumbers_1;
    phoneNumbers_1.push_back(QStringList() << "+420 666 666 660" << "+420 666 666 661" << "+420 666 666 662");
    QList<QStringList> phoneNumbers_2;
    phoneNumbers_2.push_back(QStringList() << "+420 555 555 550" << "+420 555 555 551" << "+420 555 555 552");
    setUserContactListValueRows(E_COLUMN_EMAILS, QList<QStringList>() << phoneNumbers_0 << phoneNumbers_1 << phoneNumbers_2);
    // set lists of phone numbers
    //    setUserContactListValueRows(E_COLUMN_PHONE_NUMBERS);
    // set jobs
    setUserContactSingleValueRows(E_COLUMN_JOB, QStringList() << "Build worker" << "Cleaner" << "Killer");
    // set companies
    setUserContactSingleValueRows(E_COLUMN_COMPANY, QStringList() << "Coca-Cola" << "Google" << "Lenovo");
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
