#include "MainWindow.h"

#include "Tests/Test_GoogleContacts.h"
#include "Tests/Test_Database.h"

#include <QtTest/QtTest>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setup();

    QTest::qExec(new test::Test_GoogleContacts, argc, argv);
    std::cout << endl;
    QTest::qExec(new test::Test_Database(w.getDatabase()), argc, argv);

    return a.exec();
}
