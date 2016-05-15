#include "Test_Database.h"

#include "Data/Database.h"
#include "Data/Model/User.h"
#include "Data/Model/ContactEntry.h"
#include "Data/Model/ContactProperty.h"

#include <QtTest/QTest>

namespace test
{

Test_Database::Test_Database(data::Database* database, QObject *parent) :
    m_database(database),
    QObject(parent)
{
}

void Test_Database::test_savingUserToDatabase()
{
    UserPtr tester(new data::User());
    tester->setAccessToken("accessToken");
    tester->setRefreshToken("refreshToken");
    tester->setEmail("gc_tester@gmail.com");

    m_database->save(tester);
    
    bool isUserFound = false;
    for (UserPtr user : m_database->getUsers())
    {
        if (user->getId() == tester->getId())
        {
            QCOMPARE(user->getEmail(), tester->getEmail());
            QCOMPARE(user->getAccessToken(), tester->getAccessToken());
            QCOMPARE(user->getRefreshToken(), tester->getRefreshToken());
            isUserFound = true;
            break;
        }
    }

    QCOMPARE(isUserFound, true);

    m_database->remove(tester);
}

void Test_Database::test_savingContactEntriesToDatabase()
{
    UserPtr tester(new data::User());
    tester->setAccessToken("accessToken");
    tester->setRefreshToken("refreshToken");
    tester->setEmail("gc_tester1@gmail.com");

    m_database->save(tester);

    ContactEntryPtr contactEntryPtr(new data::ContactEntry());
    contactEntryPtr->setUser(tester);
    
    ContactPropertyPtr contactNumberProperty(new data::ContactProperty(contactEntryPtr,
                                                                     "label1",
                                                                     "+4207277222",
                                                                     data::ContactProperty::E_TYPE_PHONE_NUMBER));
    ContactPropertyPtr contactEmailProperty(new data::ContactProperty(contactEntryPtr,
                                                                     "label2",
                                                                     "gc_test@mail.cz",
                                                                     data::ContactProperty::E_TYPE_EMAIL));
    
    contactEntryPtr->addPhoneNumber(contactNumberProperty);
    contactEntryPtr->addEmail(contactEmailProperty);

    m_database->save(contactEntryPtr);

    bool isContactPhoneNumberFound = false;
    bool isContactEmailFound = false;
    QList<ContactEntryPtr> contactEntriesList = m_database->getContactEntries(tester);

    auto checkIfUserConsistNumberAndEmail = [&](ContactEntryPtr contact)
    {
        for (ContactPropertyPtr phoneNumber : contact->getPhoneNumbers())
        {
            if (phoneNumber->getLabel() == contactNumberProperty->getLabel())
            {
                QCOMPARE(phoneNumber->getValue(), contactNumberProperty->getValue());
                isContactPhoneNumberFound = true;
                break;
            }
        }

        for (ContactPropertyPtr email : contact->getEmails())
        {
            if (email->getLabel() == contactEmailProperty->getLabel())
            {
                QCOMPARE(email->getValue(), contactEmailProperty->getValue());
                isContactEmailFound = true;
                break;
            }
        }

        if (isContactPhoneNumberFound && isContactEmailFound)
        {
            QCOMPARE(isContactPhoneNumberFound && isContactEmailFound, true);
            m_database->remove(tester);
            return;
        }
    };

    if (!contactEntriesList.empty() && contactEntriesList.back()->getId() == contact->getId())
    {
        checkIfUserConsistNumberAndEmail(contactEntriesList.back());
    }

    for (ContactEntryPtr contact : contactEntriesList)
    {
        if (contact->getId() == contactEntryPtr->getId())
        {
            checkIfUserConsistNumberAndEmail(contact);
        }
    }

    QCOMPARE(isContactPhoneNumberFound && isContactEmailFound, true);
    m_database->remove(tester);
}

} // namespace test
