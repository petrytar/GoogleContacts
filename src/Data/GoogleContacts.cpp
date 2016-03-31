#include "GoogleContacts.h"

#include "Data/debugAsserts.h"
#include "Data/Database.h"
#include "Data/Model/ContactEntry.h"
#include "Data/Model/ContactProperty.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QXmlSimpleReader>
#include <QDomDocument>

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
}

void GoogleContacts::readFromXmlDom(const QString& body)
{
    QXmlSimpleReader reader;
    QDomDocument domDocument;
    QXmlInputSource source;
    source.setData(body);
    domDocument.setContent(&source, &reader);

    auto getAttrText = [](QDomNode domNode, const QString& attrName) -> QString
    {
        QDomNamedNodeMap attributes = domNode.toElement().attributes();
        if (attributes.contains(attrName))
        {
            return attributes.namedItem(attrName).toAttr().value();
        }
        return QString();
    };

    auto getTextFromFirstListElement = [](QDomNodeList domNodeList) -> QString
    {
        if (domNodeList.size())
        {
            return domNodeList.at(0).toElement().text();
        }
        return QString();
    };

    QDomNodeList entryList = domDocument.elementsByTagName("entry");
    for (int i = 0; i < entryList.size(); ++i)
    {
        ContactEntry* contactEntry = new ContactEntry(parent());
        QDomNodeList googleIdDomNodeList = entryList.at(i).toElement().elementsByTagName("id");
        if (!googleIdDomNodeList.isEmpty())
        {
            contactEntry->setGoogleContactId(googleIdDomNodeList.at(0).toElement().text());
        }

        QDomNodeList updatedTimeDomNodeList = entryList.at(i).toElement().elementsByTagName("updated");
        if (!updatedTimeDomNodeList.isEmpty())
        {
            contactEntry->setUpdatedTime(updatedTimeDomNodeList.at(0).toElement().text());
        }

        QDomNodeList titleDomNodeList = entryList.at(i).toElement().elementsByTagName("title");
        if (!titleDomNodeList.isEmpty())
        {
            contactEntry->setTitle(titleDomNodeList.at(0).toElement().text());
        }

        QDomNodeList contentDomNodeList = entryList.at(i).toElement().elementsByTagName("content");
        if (!contentDomNodeList.isEmpty())
        {
            contactEntry->setContent(contentDomNodeList.at(0).toElement().text());
        }

        QDomNodeList nicknameDomNodeList = entryList.at(i).toElement().elementsByTagName("nickname");
        if (!nicknameDomNodeList.isEmpty())
        {
            contactEntry->setNickname(nicknameDomNodeList.at(0).toElement().text());
        }

        QDomNodeList fileAsDomNodeList = entryList.at(i).toElement().elementsByTagName("fileAs");
        if (!fileAsDomNodeList.isEmpty())
        {
            contactEntry->setFileAs(fileAsDomNodeList.at(0).toElement().text());
        }

        QDomNodeList orgNameDomNodeList = entryList.at(i).toElement().elementsByTagName("orgName");
        if (!orgNameDomNodeList.isEmpty())
        {
            contactEntry->setOrgName(orgNameDomNodeList.at(0).toElement().text());
        }

        QDomNodeList orgTitleDomNodeList = entryList.at(i).toElement().elementsByTagName("orgTitle");
        if (!orgTitleDomNodeList.isEmpty())
        {
            contactEntry->setOrgTitle(orgTitleDomNodeList.at(0).toElement().text());
        }

        QDomNodeList givenNameDomNodeList = entryList.at(i).toElement().elementsByTagName("givenName");
        if (!givenNameDomNodeList.isEmpty())
        {
            contactEntry->setGivenName(givenNameDomNodeList.at(0).toAttr().value(), givenNameDomNodeList.at(0).toElement().text(), "givenName");
        }

        QDomNodeList familyNameDomNodeList = entryList.at(i).toElement().elementsByTagName("familyName");
        if (!familyNameDomNodeList.isEmpty())
        {
            contactEntry->setGivenName(familyNameDomNodeList.at(0).toAttr().value(), familyNameDomNodeList.at(0).toElement().text(), "familyName");
        }

        QDomNodeList emailDomNodeList = entryList.at(i).toElement().elementsByTagName("email");
        for (int i = 0; i < emailDomNodeList.size(); ++i)
        {
            contactEntry->addEmail(getAttrText(emailDomNodeList.at(i), "label"), getAttrText(emailDomNodeList.at(i), "address"), "email");
        }

        QDomNodeList imDomNodeList = entryList.at(i).toElement().elementsByTagName("im");
        for (int i = 0; i < imDomNodeList.size(); ++i)
        {
            contactEntry->addIm(getAttrText(imDomNodeList.at(i), "protocol"), getAttrText(imDomNodeList.at(i), "address"), "im");
        }

        QDomNodeList phoneNumberDomNodeList = entryList.at(i).toElement().elementsByTagName("phoneNumber");
        for (int i = 0; i < phoneNumberDomNodeList.size(); ++i)
        {
            contactEntry->addPhoneNumber(getAttrText(phoneNumberDomNodeList.at(i), "label"), getAttrText(phoneNumberDomNodeList.at(i), "uri"), "phoneNumber");
        }

        QDomNodeList structuredPostalAddressDomNodeList = entryList.at(i).toElement().elementsByTagName("structuredPostalAddress");
        for (int i = 0; i < structuredPostalAddressDomNodeList.size(); ++i)
        {
            contactEntry->addStructuredPostalAddress(getAttrText(structuredPostalAddressDomNodeList.at(i), "label"), getTextFromFirstListElement(structuredPostalAddressDomNodeList.at(i).toElement().elementsByTagName("formattedAddress")), "structuredPostalAddress");
        }

        QDomNodeList relationDomNodeList = entryList.at(i).toElement().elementsByTagName("relation");
        for (int i = 0; i < relationDomNodeList.size(); ++i)
        {
            contactEntry->addRelation(getAttrText(relationDomNodeList.at(i), "rel"), relationDomNodeList.at(i).toElement().text(), "relation");
        }

        QDomNodeList userDefinedFieldDomNodeList = entryList.at(i).toElement().elementsByTagName("userDefinedField");
        for (int i = 0; i < userDefinedFieldDomNodeList.size(); ++i)
        {
            contactEntry->addUserDefinedField(getAttrText(userDefinedFieldDomNodeList.at(i), "key"), getAttrText(userDefinedFieldDomNodeList.at(i), "value"), "userDefinedField");
        }

        QDomNodeList websiteDomNodeList = entryList.at(i).toElement().elementsByTagName("website");
        for (int i = 0; i < websiteDomNodeList.size(); ++i)
        {
            contactEntry->addWebsite(getAttrText(websiteDomNodeList.at(i), "label"), getAttrText(websiteDomNodeList.at(i), "href"), "website");
        }

        QDomNodeList groupMembershipInfoListDomNodeList = entryList.at(i).toElement().elementsByTagName("groupMembershipInfo");
        for (int i = 0; i < groupMembershipInfoListDomNodeList.size(); ++i)
        {
            contactEntry->addGroupMembershipInfo(getAttrText(groupMembershipInfoListDomNodeList.at(i), "deleted"), getAttrText(groupMembershipInfoListDomNodeList.at(i), "href"), "groupMembershipInfo");
        }
    }
}

} // namespace data
