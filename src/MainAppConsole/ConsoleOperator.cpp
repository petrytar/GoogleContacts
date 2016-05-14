#include "ConsoleOperator.h"

#include "MainAppConsole/MainAppConsole_global.h"
#include "Data/Database.h"
#include "Data/GoogleContacts.h"
#include "Data/Auth/AuthManager.h"
#include "Data/Model/User.h"
#include "Data/Model/ContactEntry.h"

#include <QNetworkAccessManager>
#include <iostream>

ConsoleOperator::ConsoleOperator(QObject* parent) :
    BaseClass(parent),
    m_networkAccessManager(new QNetworkAccessManager(this)),
    m_authManager(new data::AuthManager(m_networkAccessManager, this)),
    m_database(new data::Database(this)),
    m_googleContacts(new data::GoogleContacts(m_networkAccessManager, m_database, this)),
    m_firstAuthorizationAttempt(true)
{
    m_database->open();

    VERIFY(connect(m_authManager, SIGNAL(error()), this, SLOT(onAuthFailed())));
    VERIFY(connect(m_authManager, SIGNAL(accessTokenReceived(QString)), this, SLOT(onAccessTokenReceived(QString))));
    VERIFY(connect(m_authManager, SIGNAL(invalidRefreshToken()), this, SLOT(onInvalidRefreshToken())));
    VERIFY(connect(m_googleContacts, SIGNAL(groupsSyncSuccessful()), this, SLOT(onGroupsSyncSuccessful())));
    //VERIFY(connect(m_googleContacts, SIGNAL(contactsSyncSuccessful()), this, SLOT(onContactsSyncSuccessful())));
    VERIFY(connect(m_googleContacts, SIGNAL(authorizationError()), this, SLOT(onContactsAuthorizationError())));
    VERIFY(connect(m_googleContacts, SIGNAL(otherError(QNetworkReply::NetworkError)), this, SLOT(onContactsOtherError(QNetworkReply::NetworkError))));
}

void ConsoleOperator::findContacts(const QString& str)
{
    auto findCallBack = [this, str]()
    {
        QList<data::ptr<data::ContactEntry>> contactEntries = m_googleContacts->findContacts(str);
        if (contactEntries.empty())
        {
            qStdOut() << "No contact entries found" << endl;
        }
        else
        {
            qStdOut() << "Contact entries found:" << endl;
            for (auto contactEntry : contactEntries)
            {
                qStdOut() << "Name: \"" << contactEntry->getVisibleName() << "\" "
                          << "Email: \"" << contactEntry->getPrimaryEmail() << "\" "
                          << "Phone: \"" << contactEntry->getPrimaryPhoneNumber() << "\"" << endl;
            }
        }
        QCoreApplication::exit();
    };
    VERIFY(connect(m_googleContacts, &data::GoogleContacts::contactsSyncSuccessful, findCallBack));
    syncContacts();
}

void ConsoleOperator::syncContacts()
{
    auto users = m_database->getUsers();
    if (users.empty())
    {
        qStdOut() << "You are not logged into any user account! Please log in through GoogleContacts.exe application" << endl;
        QCoreApplication::exit(-1);
        return;
    }

    data::ptr<data::User> user = users.at(0);
    m_googleContacts->setActiveUser(user);
    m_googleContacts->syncGroupsAndContacts();
}

void ConsoleOperator::onGroupsSyncSuccessful()
{
    qInfo() << "Contact groups successfully synchronized";
}

void ConsoleOperator::onContactsSyncSuccessful()
{
    qInfo() << "Contact entries successfully synchronized";
    QCoreApplication::exit(0);
}

void ConsoleOperator::onContactsAuthorizationError()
{
    qInfo() << "Failed to authorize user!" << endl;
    if (m_firstAuthorizationAttempt)
    {
        qInfo() << "Requesting refresh token" << endl;
        m_authManager->refreshAccessToken(m_googleContacts->getActiveUser()->getRefreshToken());
    }
    else
    {
        m_firstAuthorizationAttempt = false;
        QCoreApplication::exit(-1);
    }
}

void ConsoleOperator::onContactsOtherError(QNetworkReply::NetworkError error)
{
    qStdOut() << "Failed to sync contacts (network failure)!" << endl;
    QCoreApplication::exit(-1);
}

void ConsoleOperator::onAuthFailed()
{
    qStdOut() << "Failed to authorize user (network failure)!" << endl;
    QCoreApplication::exit(-1);
}

void ConsoleOperator::onAccessTokenReceived(const QString& accessToken)
{
    m_googleContacts->getActiveUser()->setAccessToken(accessToken);
    m_database->save(m_googleContacts->getActiveUser());
    m_googleContacts->syncGroupsAndContacts();
}

void ConsoleOperator::onInvalidRefreshToken()
{
    qStdOut() << "Failed to refresh access token (invalid refresh token)!" << endl;
    QCoreApplication::exit(-1);
}
