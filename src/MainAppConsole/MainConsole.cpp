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

    ConsoleOperator consoleOperator;
    if (!consoleOperator.init())
    {
        return -1;
    }

    bool findCommand = false;
    QString findArgument;

    bool createCommand = false;
    QString createArgument;

    bool deleteCommand = false;
    QString deleteArgument;

    QStringList args = a.arguments();
    for (int i = 0; i < args.size(); ++i)
    {
        const QString& argument = args[i];
        if (argument == "-f")
        {
            if (findCommand)
            {
                qStdOut() << "Multiple commands specified";
                return -1;
            }

            findCommand = true;
            ++i;
            if (i >= args.size())
            {
                qStdOut() << "Missing argument for find command";
                return -1;
            }
            findArgument = args[i];
        }
        else if (argument == "-c")
        {
            if (createCommand)
            {
                qStdOut() << "Multiple commands specified";
                return -1;
            }

            createCommand = true;
            ++i;
            if (i >= args.size())
            {
                qStdOut() << "Missing argument for create command";
                return -1;
            }
            createArgument = args[i];
        }
        else if (argument == "-d")
        {
            if (deleteCommand)
            {
                qStdOut() << "Multiple commands specified";
                return -1;
            }

            deleteCommand = true;
            ++i;
            if (i >= args.size())
            {
                qStdOut() << "Missing argument for delete command";
                return -1;
            }
            deleteArgument = args[i];
        }
    }

    if (findCommand + createCommand + deleteCommand > 1)
    {
        qStdOut() << "Multiple commands specified";
        return -1;
    }

    std::function<void()> operation;
    if (findCommand)
    {
        operation = [findArgument, &consoleOperator]()
        {
            consoleOperator.findContacts(findArgument);
        };
    }
    else if (createCommand)
    {
        operation = [createArgument, &consoleOperator]()
        {
            consoleOperator.createContact(createArgument);
        };
    }
    else if (deleteCommand)
    {
        operation = [deleteArgument, &consoleOperator]()
        {
            consoleOperator.deleteContacts(deleteArgument);
        };
    }
    else
    {
        qStdOut() << "Command not specified";
        return -1;
    }

    QTimer::singleShot(0, operation);

    return a.exec();
}
