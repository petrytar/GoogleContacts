#ifndef TEST_DATABASE_H
#define TEST_DATABASE_H

#include <QObject>

namespace data
{
class Database;
}

namespace test
{

class Test_Database : public QObject
{
    Q_OBJECT
public:
    explicit Test_Database(data::Database* database, QObject *parent = 0);

signals:

private slots:
    void test_savingUserToDatabase();
    void test_savingContactEntriesToDatabase();

private:
    data::Database* m_database;
};

} // namespece test

#endif // TEST_DATABASE_H
