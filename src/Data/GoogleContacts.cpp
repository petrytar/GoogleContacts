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

namespace data
{

GoogleContacts::GoogleContacts(Database* database, QObject* parent) :
    BaseClass(parent),
    m_networkAccessManager(new QNetworkAccessManager(this)),
    m_database(database)
{
    m_database->open();
    //m_user = m_database->getUser();
    //m_user = ptr<User>(new User());
    //VERIFY(connect(this, SIGNAL(userDataChanged(User*)), m_database, SLOT(onUserDataChanged(User*))));
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

    /*auto getAttrText = [](QDomNode domNode, const QString& attrName) -> QString
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
    };*/

    auto createProperty = [](ptr<ContactEntry> parentEntry, QDomNode node, const QString& nameAttribute, const QString& valueAttribute, const QString& type) -> ContactPropertyPtr
    {
        QDomElement element = node.toElement();
        ptr<ContactProperty> property(new ContactProperty(parentEntry, element.attribute(nameAttribute), element.attribute(valueAttribute), type));
        return property;
    };

    QString userEmail = domDocument.elementsByTagName("id").at(0).toElement().text();
    saveUserEmail(userEmail);

    QDomNodeList entryList = domDocument.elementsByTagName("entry");
    for (int i = 0; i < entryList.size(); ++i)
    {
        QDomElement entryElement = entryList.at(i).toElement();

        ptr<ContactEntry> contactEntry(new ContactEntry());
        QDomNodeList googleIdDomNodeList = entryElement.elementsByTagName("id");
        if (!googleIdDomNodeList.isEmpty())
        {
            contactEntry->setGoogleContactId(googleIdDomNodeList.at(0).toElement().text());
        }

        QDomNodeList updatedTimeDomNodeList = entryElement.elementsByTagName("updated");
        if (!updatedTimeDomNodeList.isEmpty())
        {
            contactEntry->setUpdatedTime(updatedTimeDomNodeList.at(0).toElement().text());
        }

        QDomNodeList titleDomNodeList = entryElement.elementsByTagName("title");
        if (!titleDomNodeList.isEmpty())
        {
            contactEntry->setTitle(titleDomNodeList.at(0).toElement().text());
        }

        QDomNodeList contentDomNodeList = entryElement.elementsByTagName("content");
        if (!contentDomNodeList.isEmpty())
        {
            contactEntry->setContent(contentDomNodeList.at(0).toElement().text());
        }

        QDomNodeList nicknameDomNodeList = entryElement.elementsByTagName("nickname");
        if (!nicknameDomNodeList.isEmpty())
        {
            contactEntry->setNickname(nicknameDomNodeList.at(0).toElement().text());
        }

        QDomNodeList fileAsDomNodeList = entryElement.elementsByTagName("fileAs");
        if (!fileAsDomNodeList.isEmpty())
        {
            contactEntry->setFileAs(fileAsDomNodeList.at(0).toElement().text());
        }

        QDomNodeList orgNameDomNodeList = entryElement.elementsByTagName("orgName");
        if (!orgNameDomNodeList.isEmpty())
        {
            contactEntry->setOrgName(orgNameDomNodeList.at(0).toElement().text());
        }

        QDomNodeList orgTitleDomNodeList = entryElement.elementsByTagName("orgTitle");
        if (!orgTitleDomNodeList.isEmpty())
        {
            contactEntry->setOrgTitle(orgTitleDomNodeList.at(0).toElement().text());
        }

        QDomNodeList nameDomNodeList = entryElement.elementsByTagName("name");
        if (!nameDomNodeList.isEmpty())
        {
            QDomNodeList fullNameDomNodeList = entryElement.elementsByTagName("fullName");
            if (!fullNameDomNodeList.isEmpty())
            {
                contactEntry->setName(fullNameDomNodeList.at(0).toElement().text());
            }
        }

        /*QDomNodeList givenNameDomNodeList = entryElement.elementsByTagName("givenName");
        if (!givenNameDomNodeList.isEmpty())
        {
            contactEntry->setGivenName(givenNameDomNodeList.at(0).toAttr().value(), givenNameDomNodeList.at(0).toElement().text(), "givenName");
        }

        QDomNodeList familyNameDomNodeList = entryElement.elementsByTagName("familyName");
        if (!familyNameDomNodeList.isEmpty())
        {
            contactEntry->setFamilyName(familyNameDomNodeList.at(0).toAttr().value(), familyNameDomNodeList.at(0).toElement().text(), "familyName");
        }*/

        QDomNodeList emailDomNodeList = entryElement.elementsByTagName("email");
        for (int i = 0; i < emailDomNodeList.size(); ++i)
        {
            contactEntry->addEmail(createProperty(contactEntry, emailDomNodeList.at(i), "label", "address", "email"));
        }

        /*QDomNodeList imDomNodeList = entryElement.elementsByTagName("im");
        for (int i = 0; i < imDomNodeList.size(); ++i)
        {
            contactEntry->addIm(getAttrText(imDomNodeList.at(i), "protocol"), getAttrText(imDomNodeList.at(i), "address"), "im");
        }*/

        QDomNodeList phoneNumberDomNodeList = entryElement.elementsByTagName("phoneNumber");
        for (int i = 0; i < phoneNumberDomNodeList.size(); ++i)
        {
            contactEntry->addPhoneNumber(createProperty(contactEntry, phoneNumberDomNodeList.at(i), "label", "uri", "phoneNumber"));
        }

        /*QDomNodeList structuredPostalAddressDomNodeList = entryElement.elementsByTagName("structuredPostalAddress");
        for (int i = 0; i < structuredPostalAddressDomNodeList.size(); ++i)
        {
            contactEntry->addStructuredPostalAddress(getAttrText(structuredPostalAddressDomNodeList.at(i), "label"), getTextFromFirstListElement(structuredPostalAddressDomNodeList.at(i).toElement().elementsByTagName("formattedAddress")), "structuredPostalAddress");
        }

        QDomNodeList relationDomNodeList = entryElement.elementsByTagName("relation");
        for (int i = 0; i < relationDomNodeList.size(); ++i)
        {
            contactEntry->addRelation(getAttrText(relationDomNodeList.at(i), "rel"), relationDomNodeList.at(i).toElement().text(), "relation");
        }

        QDomNodeList userDefinedFieldDomNodeList = entryElement.elementsByTagName("userDefinedField");
        for (int i = 0; i < userDefinedFieldDomNodeList.size(); ++i)
        {
            contactEntry->addUserDefinedField(getAttrText(userDefinedFieldDomNodeList.at(i), "key"), getAttrText(userDefinedFieldDomNodeList.at(i), "value"), "userDefinedField");
        }

        QDomNodeList websiteDomNodeList = entryElement.elementsByTagName("website");
        for (int i = 0; i < websiteDomNodeList.size(); ++i)
        {
            contactEntry->addWebsite(getAttrText(websiteDomNodeList.at(i), "label"), getAttrText(websiteDomNodeList.at(i), "href"), "website");
        }

        QDomNodeList groupMembershipInfoListDomNodeList = entryElement.elementsByTagName("groupMembershipInfo");
        for (int i = 0; i < groupMembershipInfoListDomNodeList.size(); ++i)
        {
            contactEntry->addGroupMembershipInfo(getAttrText(groupMembershipInfoListDomNodeList.at(i), "deleted"), getAttrText(groupMembershipInfoListDomNodeList.at(i), "href"), "groupMembershipInfo");
        }*/
        m_contacts.push_back(contactEntry);
        m_database->insert(contactEntry);
    }
}

/*void GoogleContacts::setAccessToken(const QString& accessToken)
{
    if (m_user->getAccessToken() != accessToken)
    {
        m_user->setAccessToken(accessToken);
        //emit userDataChanged(m_user);
    }
}*/

QString GoogleContacts::getAccessToken() const
{
    return m_activeUser->getAccessToken();
}

void GoogleContacts::saveUserEmail(const QString& userEmail)
{
    m_activeUser->setEmail(userEmail);
    //emit userDataChanged(m_user);
}

} // namespace data
