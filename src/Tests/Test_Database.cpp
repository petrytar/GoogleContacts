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
    
    ContactPropertyPtr contactPropertyPtr0(new data::ContactProperty(contactEntryPtr,
                                                                     "label1",
                                                                     "+4207277222",
                                                                     data::ContactProperty::E_TYPE_PHONE_NUMBER));
    ContactPropertyPtr contactPropertyPtr1(new data::ContactProperty(contactEntryPtr,
                                                                     "label2",
                                                                     "gc_test@mail.cz",
                                                                     data::ContactProperty::E_TYPE_EMAIL));
    
    contactEntryPtr->addPhoneNumber(contactPropertyPtr0);
    contactEntryPtr->addEmail(contactPropertyPtr1);

    m_database->save(contactEntryPtr);

    bool isContactPhoneNumberFound = false;
    bool isContactEmailFound = false;
    for (ContactEntryPtr contact : m_database->getContactEntries(tester))
    {
        if (contact->getId() == contactEntryPtr->getId())
        {
            for (ContactPropertyPtr phoneNumber : contact->getPhoneNumbers())
            {
                if (phoneNumber->getLabel() == contactPropertyPtr0->getLabel())
                {
                    QCOMPARE(phoneNumber->getValue(), contactPropertyPtr0->getValue());
                    isContactPhoneNumberFound = true;
                }
            }

            for (ContactPropertyPtr email : contact->getEmails())
            {
                if (email->getLabel() == contactPropertyPtr1->getLabel())
                {
                    QCOMPARE(email->getValue(), contactPropertyPtr1->getValue());
                    isContactEmailFound = true;
                }
            }
        }
    }
    QCOMPARE(isContactPhoneNumberFound && isContactEmailFound, true);

    m_database->remove(tester);
}

} // namespace test
