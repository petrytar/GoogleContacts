#include "LoginDialog.h"
#include "ui_LoginDialog.h"

#include "Data/debugAsserts.h"

#include <QMessageBox>

LoginDialog::LoginDialog(QUrl url, QWidget* parent) :
    BaseClass(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    ui->webView->setUrl(url);

    VERIFY(connect(ui->webView, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool))));
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::onLoadFinished(bool ok)
{
    if (!ok)
    {
        QMessageBox::critical(this, tr("Error"), tr("Failed to load web page!<br>Connection to the server may be missing."));
        emit loadFailed();
    }
}
