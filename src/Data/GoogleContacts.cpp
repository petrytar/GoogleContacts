#include "GoogleContacts.h"

#include "Data/debugAsserts.h"
#include "Data/Database.h"
#include "Data/Model/User.h"
#include "Data/Model/ContactGroup.h"
#include "Data/Model/ContactEntry.h"
#include "Data/Model/ContactProperty.h"

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
    m_contactGroups = m_database->getContactGroups(user);
    m_contactEntries = m_database->getContactEntries(user);
    remediateGroupsAndContacts();

    /*for (auto group : m_contactGroups)
    {
        qDebug() << "Group" << group.get();
    }

    for (auto entry : m_contactEntries)
    {
        qDebug() << "Entry" << entry.get();
        for (auto group : entry->getContactGroups())
        {
            qDebug() << "Entry Group" << group.get();
        }
    }*/
}

void GoogleContacts::remediateGroupsAndContacts()
{
    QMap<QString, ptr<ContactGroup>> groupsById;
    for (auto group : m_contactGroups)
    {
        groupsById.insert(group->getGoogleId(), group);
    }

    for (auto entry : m_contactEntries)
    {
        for (int i = 0; i < entry->getContactGroups().size(); ++i)
        {
            entry->getContactGroups()[i] = groupsById.value(entry->getContactGroups()[i]->getGoogleId());
        }
    }
}

void GoogleContacts::syncGroupsAndContacts()
{
    syncGroups();
}

void GoogleContacts::syncGroups()
{
    QNetworkRequest request(QUrl("https://www.google.com/m8/feeds/groups/default/full"));
    request.setRawHeader("GData-Version", "3.0");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(getAccessToken()).toUtf8());

    QNetworkReply* reply = m_networkAccessManager->get(request);
    auto onReplyFinished = [this, reply]()
    {
        if (finalizeAndCheckErrorsOnReply("syncGroups", reply))
        {
            processGetGroupsReply(reply);
        }
    };
    VERIFY(connect(reply, &QNetworkReply::finished, onReplyFinished));
}

void GoogleContacts::processGetGroupsReply(QNetworkReply* reply)
{
    QString xml(reply->readAll());
    QList<ptr<ContactGroup>> newContactGroups = parseContactGroups(xml);
    syncContactGroups(newContactGroups);

    emit groupsSyncSuccessful();

    syncContacts();
}

void GoogleContacts::syncContactGroups(QList<ptr<ContactGroup>> newContactGroups)
{
    //m_queuedRequests.clear();
    QList<ptr<ContactGroup>> syncedContactGroups;

    QMap<QString, ptr<ContactGroup>> currentContactGroups;
    QList<ptr<ContactGroup>> createdContactGroups;
    for (ptr<ContactGroup> contactGroup : m_contactGroups)
    {
        QString contactId = contactGroup->getGoogleId();
        if (!contactId.isEmpty())
        {
            /*if (contactGroup->isDeleted())
            {
                m_queuedRequests.push_back(Request(Request::E_TYPE_DELETE, contactGroup));
            }*/
            currentContactGroups.insert(contactId, contactGroup);
        }
        else
        {
            if (contactGroup->isDeleted())
            {
                qDebug() << "created and then deleted contact found";
                m_database->remove(contactGroup);
            }
            else
            {
                createdContactGroups.append(contactGroup);
            }
        }
    }

    QMap<QString, ptr<ContactGroup>> newContactGroupsMap;
    for (ptr<ContactGroup> contactGroup : newContactGroups)
    {
        newContactGroupsMap.insert(contactGroup->getGoogleId(), contactGroup);
    }

    for (ptr<ContactGroup> newContactGroup : newContactGroups)
    {
        ptr<ContactGroup> currentContactGroup = currentContactGroups.value(newContactGroup->getGoogleId());
        if (!currentContactGroup)
        {
            qDebug() << "new contact found";
            m_database->save(newContactGroup);
            syncedContactGroups.push_back(newContactGroup);
        }
        else
        {
            if (newContactGroup->getUpdatedTime() > currentContactGroup->getUpdatedTime())
            {
                qDebug() << "modified (remotely) contact found";
                m_database->update(currentContactGroup, newContactGroup);
            }
            /*else if (newContactGroup->getUpdatedTime() < currentContactGroup->getUpdatedTime())
            {
                qDebug() << "modified (locally) contact found";
                m_queuedRequests.push_back(Request(Request::E_TYPE_UPDATE, currentContactGroup));
            }*/
            syncedContactGroups.push_back(currentContactGroup);
        }
    }

    for (ptr<ContactGroup> currentContactGroup : currentContactGroups)
    {
        if (!newContactGroupsMap.contains(currentContactGroup->getGoogleId()))
        {
            qDebug() << "deleted contact found";
            m_database->remove(currentContactGroup);

            // all references to this group should be removed from entries as well
            for (auto contactEntry : m_contactEntries)
            {
                if (contactEntry->removeContactGroup(currentContactGroup))
                {
                    m_database->save(contactEntry);
                }
            }
        }
    }

    syncedContactGroups.append(createdContactGroups);
    m_contactGroups = syncedContactGroups;

    /*for (ptr<ContactGroup> createdContactGroup : createdContactGroups)
    {
        m_queuedRequests.push_back(Request(Request::E_TYPE_CREATE, createdContactGroup));
    }

    sendNextQueuedRequest();*/
}

void GoogleContacts::syncContacts()
{
    QNetworkRequest request(QUrl("https://www.google.com/m8/feeds/contacts/default/full?max-results=2000"));
    request.setRawHeader("GData-Version", "3.0");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(getAccessToken()).toUtf8());

    QNetworkReply* reply = m_networkAccessManager->get(request);
    auto onReplyFinished = [this, reply]()
    {
        if (finalizeAndCheckErrorsOnReply("loadContacts", reply))
        {
            processGetContactsReply(reply);
        }
    };
    VERIFY(connect(reply, &QNetworkReply::finished, onReplyFinished));
}

void GoogleContacts::processGetContactsReply(QNetworkReply* reply)
{
    QString body(reply->readAll());
    QList<ptr<ContactEntry>> newContactEntries = parseContactEntries(body);
    syncContactEntries(newContactEntries);
}

QList<ptr<ContactGroup>> GoogleContacts::parseContactGroups(const QString& xml)
{
    //std::cout << xml.toStdString() << std::endl;
    QXmlSimpleReader reader;
    QDomDocument domDocument;
    QXmlInputSource source;
    source.setData(xml);
    domDocument.setContent(&source, &reader);

    QList<ptr<ContactGroup>> contactGroups;

    QDomNodeList groupList = domDocument.elementsByTagName("entry");
    for (int i = 0; i < groupList.size(); ++i)
    {
        QDomElement groupElement = groupList.at(i).toElement();
        ptr<ContactGroup> contactGroup(new ContactGroup());
        ContactGroup::fromXml(contactGroup, groupElement);
        contactGroup->setUser(m_activeUser);
        contactGroups.push_back(contactGroup);
    }

    return contactGroups;
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
        ContactEntry::fromXml(contactEntry, m_contactGroups, entryElement);
        contactEntry->setUser(m_activeUser);
        contactEntries.push_back(contactEntry);
    }
    
    return contactEntries;
}

void GoogleContacts::syncContactEntries(QList<ptr<ContactEntry>> newContactEntries)
{
    m_queuedRequests.clear();
    QList<ptr<ContactEntry>> syncedContactEntries;

    QMap<QString, ptr<ContactEntry>> currentContactEntries;
    QList<ptr<ContactEntry>> createdContactEntries;
    for (ptr<ContactEntry> contactEntry : m_contactEntries)
    {
        QString contactId = contactEntry->getGoogleId();
        if (!contactId.isEmpty())
        {
            if (contactEntry->isDeleted())
            {
                m_queuedRequests.push_back(Request(Request::E_TYPE_DELETE, contactEntry));
            }
            currentContactEntries.insert(contactId, contactEntry);
        }
        else
        {
            if (contactEntry->isDeleted())
            {
                qDebug() << "created and then deleted contact found";
                m_database->remove(contactEntry);
            }
            else
            {
                createdContactEntries.append(contactEntry);
            }
        }
    }

    QMap<QString, ptr<ContactEntry>> newContactEntriesMap;
    for (ptr<ContactEntry> contactEntry : newContactEntries)
    {
        newContactEntriesMap.insert(contactEntry->getGoogleId(), contactEntry);
    }

    for (ptr<ContactEntry> newContactEntry : newContactEntries)
    {
        ptr<ContactEntry> currentContactEntry = currentContactEntries.value(newContactEntry->getGoogleId());
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
                qDebug() << "modified (remotely) contact found";
                m_database->update(currentContactEntry, newContactEntry);
            }
            else if (newContactEntry->getUpdatedTime() < currentContactEntry->getUpdatedTime())
            {
                qDebug() << "modified (locally) contact found";
                m_queuedRequests.push_back(Request(Request::E_TYPE_UPDATE, currentContactEntry));
            }
            syncedContactEntries.push_back(currentContactEntry);
        }
    }

    for (ptr<ContactEntry> currentContactEntry : currentContactEntries)
    {
        if (!newContactEntriesMap.contains(currentContactEntry->getGoogleId()))
        {
            qDebug() << "deleted contact found";
            m_database->remove(currentContactEntry);
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
        emit contactsSyncSuccessful();
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
        case Request::E_TYPE_DELETE:
        {
            sendDeleteContactEntryRequest(nextRequest.contactEntry);
            break;
        }
        case Request::E_TYPE_UPDATE:
        {
            sendUpdateContactEntryRequest(nextRequest.contactEntry);
            break;
        }
        default:
        {
            assert(false);
            break;
        }
    }
}

bool GoogleContacts::finalizeAndCheckErrorsOnReply(const QString& description, QNetworkReply* reply)
{
    qDebug() << description << "finished with code:" << reply->error();
    reply->deleteLater();
    if (reply->error() == QNetworkReply::NoError)
    {
        return true;
    }
    qDebug() << "Error reply contents:";
    QString xml(reply->readAll());
    //std::cout << xml.toStdString() << std::endl;
    if (reply->error() == QNetworkReply::AuthenticationRequiredError)
    {
        qDebug() << "Access token is not valid!";
        emit authorizationError();
    }
    else
    {
        emit otherError(reply->error());
    }
    return false;
}

void GoogleContacts::sendCreateContactEntryRequest(ptr<ContactEntry> contactEntry)
{
    qDebug() << "Sending Create Contact Entry request";

    QNetworkRequest request(QUrl("https://www.google.com/m8/feeds/contacts/default/full"));
    request.setRawHeader("GData-Version", "3.0");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(getAccessToken()).toUtf8());
    request.setRawHeader("Content-Type", "application/atom+xml");

    QString xml = contactEntry->toXml();
    //std::cout << xml.toStdString() << std::endl;

    QNetworkReply* reply = m_networkAccessManager->post(request, xml.toUtf8());
    auto onReplyFinished = [this, reply, contactEntry]()
    {
        if (finalizeAndCheckErrorsOnReply("sendCreateContactEntryRequest", reply))
        {
            processCreateContactEntryReply(contactEntry, reply);
            sendNextQueuedRequest();
        }
    };
    VERIFY(connect(reply, &QNetworkReply::finished, onReplyFinished));
}

void GoogleContacts::processCreateContactEntryReply(ptr<ContactEntry> contactEntry, QNetworkReply* reply)
{
    qDebug() << "Create contact reply:";
    QString xml(reply->readAll());
    //std::cout << xml.toStdString() << std::endl;
    QList<ptr<ContactEntry>> createContactEntries = parseContactEntries(xml);
    assert(createContactEntries.size() == 1);
    m_database->update(contactEntry, createContactEntries.at(0));
}

void GoogleContacts::sendDeleteContactEntryRequest(ptr<ContactEntry> contactEntry)
{
    qDebug() << "Sending Delete Contact Entry request";

    QNetworkRequest request(QUrl(QString("https://www.google.com/m8/feeds/contacts/default/full/") + contactEntry->getGoogleShortId()));
    request.setRawHeader("GData-Version", "3.0");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(getAccessToken()).toUtf8());
    request.setRawHeader("If-Match", "*");

    QNetworkReply* reply = m_networkAccessManager->deleteResource(request);
    auto onReplyFinished = [this, reply, contactEntry]()
    {
        if (finalizeAndCheckErrorsOnReply("sendDeleteContactEntryRequest", reply))
        {
            processDeleteContactEntryReply(contactEntry, reply);
            sendNextQueuedRequest();
        }
    };
    VERIFY(connect(reply, &QNetworkReply::finished, onReplyFinished));
}

void GoogleContacts::processDeleteContactEntryReply(ptr<ContactEntry> contactEntry, QNetworkReply* reply)
{
    m_database->remove(contactEntry);
    m_contactEntries.removeOne(contactEntry);
}

void GoogleContacts::sendUpdateContactEntryRequest(ptr<ContactEntry> contactEntry)
{
    qDebug() << "Sending Update Contact Entry request";

    QNetworkRequest request(QUrl(QString("https://www.google.com/m8/feeds/contacts/default/full/") + contactEntry->getGoogleShortId()));
    request.setRawHeader("GData-Version", "3.0");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(getAccessToken()).toUtf8());
    request.setRawHeader("Content-Type", "application/atom+xml");
    request.setRawHeader("If-Match", "*");

    QString xml = contactEntry->toXml();
    //std::cout << xml.toStdString() << std::endl;

    QNetworkReply* reply = m_networkAccessManager->put(request, xml.toUtf8());
    auto onReplyFinished = [this, reply, contactEntry]()
    {
        if (finalizeAndCheckErrorsOnReply("sendUpdateContactEntryRequest", reply))
        {
            processUpdateContactEntryReply(contactEntry, reply);
            sendNextQueuedRequest();
        }
    };
    VERIFY(connect(reply, &QNetworkReply::finished, onReplyFinished));
}

void GoogleContacts::processUpdateContactEntryReply(ptr<ContactEntry> contactEntry, QNetworkReply* reply)
{
    qDebug() << "Update contact reply:";
    QString xml(reply->readAll());
    //std::cout << xml.toStdString() << std::endl;
    QList<ptr<ContactEntry>> updatedContactEntries = parseContactEntries(xml);
    assert(updatedContactEntries.size() == 1);
    m_database->update(contactEntry, updatedContactEntries.at(0));
}

QList<ptr<ContactEntry>> GoogleContacts::findContacts(const QString& str)
{
    QRegExp regExp(str);
    regExp.setCaseSensitivity(Qt::CaseInsensitive);
    regExp.setPatternSyntax(QRegExp::Wildcard);

    QList<ptr<ContactEntry>> foundContactEntries;
    for (ptr<ContactEntry> contactEntry : m_contactEntries)
    {
        if (contactEntry->getVisibleName().contains(regExp)
                || contactEntry->getPrimaryEmail().contains(regExp)
                || contactEntry->getPrimaryPhoneNumber().contains(regExp))
        {
            foundContactEntries.append(contactEntry);
        }
    }
    return foundContactEntries;
}

bool GoogleContacts::exportContacts(const QString& fileName)
{
    QDomDocument document;
    document.appendChild(document.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));
    QDomElement element = document.createElement("entries");
    document.appendChild(element);
    for (ptr<ContactEntry> contactEntry : m_contactEntries)
    {
        contactEntry->toXmlDom(element);
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file for writing.";
        return false;
    }

    QTextStream stream(&file);
    stream << document.toString(2);
    file.close();
    return true;
}

bool GoogleContacts::importContacts(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file for reading.";
        return false;
    }

    QTextStream stream(&file);
    QList<ptr<ContactEntry>> contactEntries = parseContactEntries(stream.readAll());
    for (auto contactEntry : contactEntries)
    {
        contactEntry->setGoogleId(QString());
        contactEntry->setUpdatedTime(QDateTime::currentDateTime());
        addContact(contactEntry);
    }
    return true;
}

} // namespace data
