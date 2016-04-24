#include "GoogleContacts.h"

#include "Data/debugAsserts.h"
#include "Data/Database.h"
#include "Data/Model/ContactProperty.h"
#include "Data/Model/User.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QXmlSimpleReader>
#include <QDomDocument>
#include <QDomElement>
#include <QMap>

namespace data
{

GoogleContacts::GoogleContacts(QNetworkAccessManager* networkAccessManager, Database* database, QObject* parent) :
    BaseClass(parent),
    m_networkAccessManager(networkAccessManager),
    m_database(database)
{
}

GoogleContacts::~GoogleContacts()
{
}

void GoogleContacts::setActiveUser(ptr<User> user)
{
    m_activeUser = user;
    m_contactEntries = m_database->getContactEntries(user);
}

void GoogleContacts::loadContacts()
{
    QNetworkRequest request(QUrl("https://www.google.com/m8/feeds/contacts/default/full"));
    request.setRawHeader("GData-Version", "3.0");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(getAccessToken()).toUtf8());

    QNetworkReply* reply = m_networkAccessManager->get(request);
    VERIFY(connect(reply, SIGNAL(finished()), this, SLOT(onReplyFinished())));
}

void GoogleContacts::onReplyFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Reply from Contacts API ended with error:" << reply->error();
        emit contactsLoadFailed();
        return;
    }

    QString body(reply->readAll());
    QList<ptr<ContactEntry>> newContactEntries = parseContactEntries(body);
    syncContactEntries(newContactEntries);
}

QList<ptr<ContactEntry>> GoogleContacts::parseContactEntries(const QString& xml)
{
    //std::cout << xml.toStdString() << std::endl;
    QXmlSimpleReader reader;
    QDomDocument domDocument;
    QXmlInputSource source;
    source.setData(xml);
    domDocument.setContent(&source, &reader);

    QList<ptr<ContactEntry>> contactEntries;

    QDomNodeList entryList = domDocument.elementsByTagName("entry");
    for (int i = 0; i < entryList.size(); ++i)
    {
        QDomElement entryElement = entryList.at(i).toElement();
        ptr<ContactEntry> contactEntry(new ContactEntry());
        ContactEntry::fromXml(contactEntry, entryElement);
        contactEntry->setUser(m_activeUser);
        contactEntries.push_back(contactEntry);
    }
    
    return contactEntries;
}

void GoogleContacts::syncContactEntries(QList<ptr<ContactEntry>> newContactEntries)
{
    m_queuedRequests.clear();

    QMap<QString, ptr<ContactEntry>> currentContactEntries;
    QList<ptr<ContactEntry>> createdContactEntries;
    for (ptr<ContactEntry> contactEntry : m_contactEntries)
    {
        QString contactId = contactEntry->getGoogleContactId();
        if (!contactId.isEmpty())
        {
            currentContactEntries.insert(contactId, contactEntry);
        }
        else
        {
            createdContactEntries.append(contactEntry);
        }
    }

    QList<ptr<ContactEntry>> syncedContactEntries;

    for (ptr<ContactEntry> newContactEntry : newContactEntries)
    {
        ptr<ContactEntry> currentContactEntry = currentContactEntries.value(newContactEntry->getGoogleContactId());
        if (!currentContactEntry)
        {
            qDebug() << "new contact found";
            m_database->save(newContactEntry);
            syncedContactEntries.push_back(newContactEntry);
        }
        else
        {
            if (newContactEntry->getUpdatedTime() > currentContactEntry->getUpdatedTime())
            {
                qDebug() << "modified contact found";
                m_database->update(currentContactEntry, newContactEntry);
            }
            syncedContactEntries.push_back(currentContactEntry);
        }
    }

    syncedContactEntries.append(createdContactEntries);
    m_contactEntries = syncedContactEntries;

    for (ptr<ContactEntry> createdContactEntry : createdContactEntries)
    {
        m_queuedRequests.push_back(Request(Request::E_TYPE_CREATE, createdContactEntry));
    }

    sendNextQueuedRequest();
}

QString GoogleContacts::getAccessToken() const
{
    return m_activeUser->getAccessToken();
}

void GoogleContacts::addContact(ptr<ContactEntry> contactEntry)
{
    contactEntry->setUser(getActiveUser());
    m_database->save(contactEntry);
    m_contactEntries.append(contactEntry);
}

void GoogleContacts::sendNextQueuedRequest()
{
    if (m_queuedRequests.isEmpty())
    {
        emit contactsLoad();
        return;
    }

    Request nextRequest = m_queuedRequests.takeFirst();
    switch (nextRequest.type)
    {
        case Request::E_TYPE_CREATE:
        {
            sendCreateContactEntryRequest(nextRequest.contactEntry);
            break;
        }
        default:
        {
            assert(false);
            break;
        }
    }
}

void GoogleContacts::sendCreateContactEntryRequest(ptr<ContactEntry> contactEntry)
{
    qDebug() << "Sending Create Contact Entry request";

    QNetworkRequest request(QUrl("https://www.google.com/m8/feeds/contacts/default/full"));
    request.setRawHeader("GData-Version", "3.0");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(getAccessToken()).toUtf8());
    request.setRawHeader("Content-Type", "application/atom+xml");

    QString xml = contactEntry->toXml();
    std::cout << xml.toStdString() << std::endl;

    QNetworkReply* reply = m_networkAccessManager->post(request, xml.toUtf8());
    auto onReplyFinished = [this, reply, contactEntry]()
    {
        reply->deleteLater();
        processCreateContactEntryReply(contactEntry, reply);
        sendNextQueuedRequest();
    };
    VERIFY(connect(reply, &QNetworkReply::finished, onReplyFinished));
}

void GoogleContacts::processCreateContactEntryReply(ptr<ContactEntry> contactEntry, QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "onCreateContactEntryRequestFinished: Reply from Contacts API ended with error:" << reply->error();
        return;
    }

    qDebug() << "onCreateContactEntryRequestFinished: finished successfully";
    QString xml(reply->readAll());
    std::cout << xml.toStdString() << std::endl;
    QList<ptr<ContactEntry>> createContactEntries = parseContactEntries(xml);
    assert(createContactEntries.size() == 1);
    m_database->update(contactEntry, createContactEntries.at(0));
}

} // namespace data
