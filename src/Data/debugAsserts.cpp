#include "debugAsserts.h"

#include "Data/debugBreak.h"

#include <QDebug>
#include <QMessageBox>
#include <QApplication>
#include <QStringList>

void switchableAssertGCC(const char* fileName, int lineNumber, const char* assertString)
{
    QString assertionText = "Assertion failed!\nfile: %1\nline: %2\nAssert condition: %3";
    assertionText = assertionText.arg(fileName).arg(lineNumber).arg(assertString);
    qDebug() << assertionText;
    bool showDialog = true;
    int ret = QMessageBox::Retry;

    if (QApplication::instance())
    {
        static QStringList args = QApplication::arguments();
        if (args.contains("--DEV-no-assert-dialog", Qt::CaseInsensitive))
        {
            showDialog = false;
        }
        if (args.contains("--DEV-assert-ignore", Qt::CaseInsensitive))
        {
            showDialog = false;
            ret = QMessageBox::Ignore;
        }
    }
    else
    {
        showDialog = false;
    }

    if (showDialog)
    {
        QMessageBox msgBox;
        msgBox.setWindowModality(Qt::ApplicationModal);
        msgBox.setWindowTitle("Assert");
        msgBox.setText(assertionText);
        #ifdef NDEBUG
        // release mod
        msgBox.setStandardButtons(QMessageBox::Abort | QMessageBox::Ignore);
        msgBox.setDefaultButton(QMessageBox::Ignore);
        #else
        msgBox.setStandardButtons(QMessageBox::Abort | QMessageBox::Retry | QMessageBox::Ignore);
        msgBox.setDefaultButton(QMessageBox::Retry);
        #endif //NDEBUG
        ret = msgBox.exec();

        if (ret == QMessageBox::Abort)
        {
            exit(-1);
        }
        else if (ret == QMessageBox::Retry)
        {
            debug_break(); // stops debugger with the possibility to continue
            return;
        }
        else
        {
            return;
        }
    }
    return;
}
