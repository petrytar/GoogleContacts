#ifndef TEST_GOOGLECONTACTS_H
#define TEST_GOOGLECONTACTS_H

#include <QObject>
#include <QNetworkReply>

namespace data
{
class GoogleContacts;
}

namespace test
{

class Test_GoogleContacts : public QObject
{
    Q_OBJECT
public:
    explicit Test_GoogleContacts(QObject* parent = 0);

private slots:
    void test_finalizeAndCheckErrorsOnReply_validInput();
    void test_finalizeAndCheckErrorsOnReply_invalidInput();
    void test_parseContactEntries_validInput();
    void test_parseContactEntries_invalidInput();
};

} // namespace test
#endif // TEST_GOOGLECONTACTS_H
