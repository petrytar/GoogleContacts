#include "Test_GoogleContacts.h"

#include "Data/GoogleContacts.h"
#include "Data/Database.h"
#include "Data/debugAsserts.h"

#include <QNetworkAccessManager>
#include <QtTest/QTest>
#include <QString>
#include <QFile>
#include <QCoreApplication>

namespace test
{

Test_GoogleContacts::Test_GoogleContacts(QObject* parent) : QObject(parent)
{
}

void Test_GoogleContacts::test_finalizeAndCheckErrorsOnReply_validInput()
{
    QNetworkAccessManager networkAccessManager;

    auto onFinished = [this](QNetworkReply* reply)
    {
        data::GoogleContacts* googleContacts = new data::GoogleContacts(new QNetworkAccessManager(this), new data::Database(this), this);
        QCOMPARE(googleContacts->finalizeAndCheckErrorsOnReply(QString("Test google contacts"), reply), true);
    };

    VERIFY(connect(&networkAccessManager, &QNetworkAccessManager::finished, onFinished));
    networkAccessManager.get(QNetworkRequest(QUrl("http://google.com")));
}

void Test_GoogleContacts::test_finalizeAndCheckErrorsOnReply_invalidInput()
{
    QNetworkAccessManager networkAccessManager;

    auto onFinished = [this](QNetworkReply* reply)
    {
        data::GoogleContacts* googleContacts = new data::GoogleContacts(new QNetworkAccessManager(this), new data::Database(this), this);
        QCOMPARE(googleContacts->finalizeAndCheckErrorsOnReply(QString("Test google contacts"), reply), false);
    };

    VERIFY(connect(&networkAccessManager, &QNetworkAccessManager::finished, onFinished));
    networkAccessManager.get(QNetworkRequest(QUrl("invalid")));
}

void Test_GoogleContacts::test_parseContactEntries_validInput()
{
    data::GoogleContacts* googleContacts = new data::GoogleContacts(new QNetworkAccessManager(this), new data::Database(this), this);

    QString xml;
    QString fileName = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("../../src/Tests/ContactValidEntries.xml");

    QFile xmlFile(fileName);
    ASSERT(xmlFile.exists(), "ContactValidEntries file does not exists.");
    xmlFile.open(QIODevice::ReadOnly | QIODevice::Text);
    xml = xmlFile.readAll();
    xmlFile.close();

    QCOMPARE(googleContacts->parseContactEntries(xml).size(), 3);
}

void Test_GoogleContacts::test_parseContactEntries_invalidInput()
{
    data::GoogleContacts* googleContacts = new data::GoogleContacts(new QNetworkAccessManager(this), new data::Database(this), this);

    QString xml = "Invalid xml file";

    QCOMPARE(googleContacts->parseContactEntries(xml).size(), 0);
}

} // namespace test