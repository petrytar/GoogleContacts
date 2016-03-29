#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Data/Auth/AuthManager.h"
#include "Data/GoogleContacts.h"
#include "Data/debugAsserts.h"

#include <QDesktopWidget>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) :
    BaseClass(parent),
    ui(new Ui::MainWindow),
    m_authManager(new data::AuthManager(this)),
    m_googleContacts(new data::GoogleContacts(this))
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
    if (/*false && */!isAccessTokenEnabled())
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

void MainWindow::onAuthSuccessful()
{
    m_loginDialog->close();
    m_googleContacts->setAccessToken(m_authManager->getAccessToken());
    m_googleContacts->loadContacts();
    QApplication::setOverrideCursor(Qt::WaitCursor);
    VERIFY(connect(m_googleContacts, SIGNAL(contactsLoad()), this, SLOT(onContactsLoad())));
    show();
}

void MainWindow::onContactsLoad()
{
    updateTableData();
    QApplication::restoreOverrideCursor();
}

void MainWindow::onAuthFailed()
{
    m_loginDialog->close();
    QMessageBox::critical(this, QString("Error"), QString("Failed authentication!"));
    exit(-1);
}

void MainWindow::updateTableData()
{
    ui->tableWidget->setRowCount(m_googleContacts->getContacts().size());
    ui->tableWidget->setColumnCount(data::GoogleContacts::E_TAG__END);

    for (int row = 0; row < m_googleContacts->getContacts().size(); ++row)
    {
        for (int column = data::GoogleContacts::E_TAG__BEGIN; column < data::GoogleContacts::E_TAG__END; ++column)
        {
            QList<data::Value*> values = m_googleContacts->getContacts().at(row)->getData(column);
            QString stringOfValues;
            for (auto value : values)
            {
                stringOfValues += value->getText();
                for (auto attribute : value->getAttributes())
                {
                    stringOfValues += "\n" + attribute->getAttrName() + " : " + attribute->getAttrValue();
                }
                stringOfValues += " ";
            }
            ui->tableWidget->setItem(row, column, new QTableWidgetItem(stringOfValues));
        }
    }
}
