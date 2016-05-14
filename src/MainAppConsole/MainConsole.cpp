#include <QCoreApplication>

#include "MainAppConsole/MainAppConsole_global.h"
#include "MainAppConsole/ConsoleOperator.h"

#include <QTimer>
#include <QDebug>

void noMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
}

int main(int argc, char *argv[])
{
#ifndef QT_DEBUG
    // disable all debug/warning/etc output in release build
    qInstallMessageHandler(noMessageOutput);
#endif

    QCoreApplication a(argc, argv);

    bool findCommand = false;
    QString findArgument;

    QStringList args = a.arguments();
    for (int i = 0; i < args.size(); ++i)
    {
        const QString& argument = args[i];
        if (argument == "-f")
        {
            findCommand = true;
            ++i;
            if (i >= args.size())
            {
                qStdOut() << "Missing argument for find command";
                return -1;
            }
            findArgument = args[i];
        }
    }

    ConsoleOperator consoleOperator;

    if (findCommand)
    {
        auto findConctacts = [findArgument, &consoleOperator]()
        {
            consoleOperator.findContacts(findArgument);
        };
        QTimer::singleShot(0, findConctacts);
    }
    else
    {
        qStdOut() << "Command not specified";
        return -1;
    }

    return a.exec();
}
