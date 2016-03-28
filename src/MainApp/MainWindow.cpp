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
    if (!isAccessTokenEnabled())
    {
        m_authManager->start();
        m_loginDialog = new LoginDialog(m_authManager->generateAuthorizationRequestUrl(), parentWidget());
        VERIFY(connect(m_authManager, SIGNAL(accessTokenReceived()), this, SLOT(onAuthSuccessful())));
        VERIFY(connect(m_authManager, SIGNAL(error()), this, SLOT(onAuthFailed())));
        m_loginDialog->show();
    }
    else
    {
        show();
    }
}

void MainWindow::onAuthSuccessful()
{
    m_loginDialog->close();
    m_googleContacts->setAccessToken(m_authManager->getAccessToken());
    m_googleContacts->loadContacts();
    show();
}

void MainWindow::onAuthFailed()
{
    m_loginDialog->close();
    QMessageBox::critical(this, QString("Error"), QString("Failure in authentication"));
    exit(-1);
}
