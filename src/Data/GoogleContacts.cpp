#include "GoogleContacts.h"

#include <Data/Model/Database.h>
#include "Data/debugAsserts.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QXmlSimpleReader>
#include <QDomDocument>
#include <QDomNamedNodeMap>

namespace data
{

GoogleContacts::GoogleContacts(QObject* parent) :
    BaseClass(parent),
    m_database(new Database()),
    m_accessToken(""),
    m_networkAccessManager(new QNetworkAccessManager)
{
    m_database->open();
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
    emit contactsLoad();
    printAllContacts();
}

void GoogleContacts::printAllContacts() const
{
    for (const ContactEntry* contactEntry : m_contactEntries)
    {
        auto data = contactEntry->getData();
        for (auto it = data.begin(); it != data.end(); ++it)
        {
            qDebug() << columnIdToColumnString(it.key()) << " : " << it.value().first()->getText();
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

    auto addValuesToContactEntry = [&] (ContactEntry* contactEntry, ETagId columnId, const QDomNodeList& domNodeList, bool& someDataSaved)
    {
        QList<Value*> values;
        for (int i = 0; i < domNodeList.size(); ++i)
        {
            QString tagText = domNodeList.at(i).toElement().text();
            if (tagText.isEmpty())
            {
                tagText = columnIdToColumnString(columnId);
            }

            QDomNamedNodeMap attributes = domNodeList.at(i).toElement().attributes();
            QList<data::Attribute*> attributesList;
            for (int i = 0; i < attributes.size(); ++i)
            {
                attributesList.push_back(new data::Attribute(attributes.item(i).toAttr().name(), attributes.item(i).toAttr().value()));
            }

            // save tag text and attributes to Value and add to contactEntry, set flag someDataSaved to mark that contactEntry is not empty
            data::Value* value = new data::Value(tagText, attributesList);
            values.push_back(value);
        }

        if (values.size())
        {
            contactEntry->setData(columnId, values);
            someDataSaved = true;
        }
    };


    // get one entry and start parse it
    QDomNodeList entryList = domDocument.elementsByTagName("entry");
    for (int i = 0; i < entryList.size(); ++i)
    {
        // someDataWasSaved true if contactEntry is not empty
        bool someDataWasSaved = false;
        ContactEntry* contactEntry = new ContactEntry(parent());

        // find in entry tag with name columnIdToColumnString(E_TAG_UPDATED)
        QDomNodeList updatedDomNodeList = entryList.at(i).toElement().elementsByTagName(columnIdToColumnString(E_TAG_UPDATED));
        if (!updatedDomNodeList.isEmpty())
        {
            addValuesToContactEntry(contactEntry, E_TAG_UPDATED, updatedDomNodeList, someDataWasSaved);
        }

        QDomNodeList titleDomNodeList = entryList.at(i).toElement().elementsByTagName(columnIdToColumnString(E_TAG_TITLE));
        if (!titleDomNodeList.isEmpty())
        {
            addValuesToContactEntry(contactEntry, E_TAG_TITLE, titleDomNodeList, someDataWasSaved);
        }

        QDomNodeList contentDomNodeList = entryList.at(i).toElement().elementsByTagName(columnIdToColumnString(E_TAG_CONTENT));
        if (!contentDomNodeList.isEmpty())
        {
            addValuesToContactEntry(contactEntry, E_TAG_CONTENT, contentDomNodeList, someDataWasSaved);
        }

        QDomNodeList givenNameDomNodeList = entryList.at(i).toElement().elementsByTagName(columnIdToColumnString(E_TAG_GIVEN_NAME));
        if (!givenNameDomNodeList.isEmpty())
        {
            addValuesToContactEntry(contactEntry, E_TAG_GIVEN_NAME, givenNameDomNodeList, someDataWasSaved);
        }

        QDomNodeList givenFamilyDomNodeList = entryList.at(i).toElement().elementsByTagName(columnIdToColumnString(E_TAG_FAMILY_NAME));
        if (!givenFamilyDomNodeList.isEmpty())
        {
            addValuesToContactEntry(contactEntry, E_TAG_FAMILY_NAME, givenFamilyDomNodeList, someDataWasSaved);
        }

        QDomNodeList nicknameDomNodeList = entryList.at(i).toElement().elementsByTagName(columnIdToColumnString(E_TAG_NICKNAME));
        if (!nicknameDomNodeList.isEmpty())
        {
            addValuesToContactEntry(contactEntry, E_TAG_NICKNAME, nicknameDomNodeList, someDataWasSaved);
        }

        QDomNodeList fileAsDomNodeList = entryList.at(i).toElement().elementsByTagName(columnIdToColumnString(E_TAG_FILE_AS));
        if (!fileAsDomNodeList.isEmpty())
        {
            addValuesToContactEntry(contactEntry, E_TAG_FILE_AS, fileAsDomNodeList, someDataWasSaved);
        }

        QDomNodeList orgNameDomNodeList = entryList.at(i).toElement().elementsByTagName(columnIdToColumnString(E_TAG_ORG_NAME));
        if (!orgNameDomNodeList.isEmpty())
        {
            addValuesToContactEntry(contactEntry, E_TAG_ORG_NAME, orgNameDomNodeList, someDataWasSaved);
        }

        QDomNodeList orgTitleDomNodeList = entryList.at(i).toElement().elementsByTagName(columnIdToColumnString(E_TAG_ORG_TITLE));
        if (!orgTitleDomNodeList.isEmpty())
        {
            addValuesToContactEntry(contactEntry, E_TAG_ORG_TITLE, orgTitleDomNodeList, someDataWasSaved);
        }

        QDomNodeList emailDomList = entryList.at(i).toElement().elementsByTagName(columnIdToColumnString(E_TAG_EMAIL));
        if (!emailDomList.isEmpty())
        {
            addValuesToContactEntry(contactEntry, E_TAG_EMAIL, emailDomList, someDataWasSaved);
        }

        QDomNodeList imDomList = entryList.at(i).toElement().elementsByTagName(columnIdToColumnString(E_TAG_IM));
        if (!imDomList.isEmpty())
        {
            addValuesToContactEntry(contactEntry, E_TAG_IM, imDomList, someDataWasSaved);
        }

        QDomNodeList phoneNumberDomList = entryList.at(i).toElement().elementsByTagName(columnIdToColumnString(E_TAG_PHONE_NUMBER));
        if (!phoneNumberDomList.isEmpty())
        {
            addValuesToContactEntry(contactEntry, E_TAG_PHONE_NUMBER, phoneNumberDomList, someDataWasSaved);
        }

        QDomNodeList addressDomList = entryList.at(i).toElement().elementsByTagName(columnIdToColumnString(E_TAG_STRUCTURED_POSTAL_ADDRESS));
        if (!addressDomList.isEmpty())
        {
            addValuesToContactEntry(contactEntry, E_TAG_STRUCTURED_POSTAL_ADDRESS, addressDomList, someDataWasSaved);
        }

        QDomNodeList relationDomList = entryList.at(i).toElement().elementsByTagName(columnIdToColumnString(E_TAG_RELATION));
        if (!relationDomList.isEmpty())
        {
            addValuesToContactEntry(contactEntry, E_TAG_RELATION, relationDomList, someDataWasSaved);
        }

        QDomNodeList userDefinedFieldDomList = entryList.at(i).toElement().elementsByTagName(columnIdToColumnString(E_TAG_USER_DEFINED_FIELD));
        if (!userDefinedFieldDomList.isEmpty())
        {
            addValuesToContactEntry(contactEntry, E_TAG_USER_DEFINED_FIELD, userDefinedFieldDomList, someDataWasSaved);
        }

        QDomNodeList websiteDomList = entryList.at(i).toElement().elementsByTagName(columnIdToColumnString(E_TAG_WEBSITE));
        if (!websiteDomList.isEmpty())
        {
            addValuesToContactEntry(contactEntry, E_TAG_WEBSITE, websiteDomList, someDataWasSaved);
        }

        QDomNodeList groupMembershipUnfoDomList = entryList.at(i).toElement().elementsByTagName(columnIdToColumnString(E_TAG_GROUP_MEMBERSHIP_INFO));
        if (!groupMembershipUnfoDomList.isEmpty())
        {
            addValuesToContactEntry(contactEntry, E_TAG_GROUP_MEMBERSHIP_INFO, groupMembershipUnfoDomList, someDataWasSaved);
        }

        if (contactEntry)
        {
            m_contactEntries.push_back(contactEntry);
        }
        else
        {
            delete contactEntry;
        }
    }
}

QList<ContactEntry*>& GoogleContacts::getContacts()
{
    return m_contactEntries;
}

QString GoogleContacts::columnIdToColumnString(int columnId) const
{
    switch(columnId)
    {
        case E_TAG_UPDATED:
            return "updated";
        case E_TAG_TITLE:
            return "title";
        case E_TAG_CONTENT:
            return "content";
        case E_TAG_GIVEN_NAME:
            return "givenName";
        case E_TAG_FAMILY_NAME:
            return "familyName";
        case E_TAG_NICKNAME:
            return "nickname";
        case E_TAG_FILE_AS:
            return "fileAs";
        case E_TAG_ORG_NAME:
            return "orgName";
        case E_TAG_ORG_TITLE:
            return "orgTitle";
        case E_TAG_EMAIL:
            return "email";
        case E_TAG_IM:
            return "im";
        case E_TAG_PHONE_NUMBER:
            return "phoneNumber";
        case E_TAG_STRUCTURED_POSTAL_ADDRESS:
            return "structuredPostalAddress";
        case E_TAG_RELATION:
            return "relation";
        case E_TAG_USER_DEFINED_FIELD:
            return "userDefinedField";
        case E_TAG_WEBSITE:
            return "website";
        case E_TAG_GROUP_MEMBERSHIP_INFO:
            return "groupMembershipInfo";
        default:
            ASSERT(false, "Unknown column id");
            return "unknown";
    }
}

} // namespace data
