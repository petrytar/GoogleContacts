#include "GoogleContacts.h"

#include "Data/debugAsserts.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QXmlSimpleReader>
#include <QDomDocument>

namespace data
{

GoogleContacts::GoogleContacts(QObject* parent) :
    BaseClass(parent),
    m_accessToken(""),
    m_networkAccessManager(new QNetworkAccessManager)
{
}

GoogleContacts::~GoogleContacts()
{
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
    if (reply->error() != QNetworkReply::NoError)
    {
        return;
    }

    QString body(reply->readAll());
    readFromXmlDom(body);
    printAllContacts();
}

void GoogleContacts::printAllContacts() const
{
    for (const ContactEntry* contactEntry : m_contactEntries)
    {
        auto data = contactEntry->getData();
        for (auto it = data.begin(); it != data.end(); ++it)
        {
            qDebug() << it.key() << " : " << it.value();
        }
    }
}

void GoogleContacts::readFromXmlDom(const QString& body)
{
    QXmlSimpleReader reader;
    QDomDocument domDocument;
    QXmlInputSource source;
    source.setData(body);
    domDocument.setContent(&source, &reader);

    QDomNodeList entryList = domDocument.elementsByTagName("entry");
    for (int i = 0; i < entryList.size(); ++i)
    {
        ContactEntry* contactEntry = new ContactEntry(parent());
        QDomNodeList entryNameList = entryList.at(i).toElement().elementsByTagName("name");
        QString fullName = entryNameList.at(0).toElement().elementsByTagName("fullName").at(0).toElement().text();
        if (!fullName.isEmpty())
        {
            contactEntry->setData("fullName", fullName);
            m_contactEntries.push_back(contactEntry);
        }
    }
}

} // namespace data
