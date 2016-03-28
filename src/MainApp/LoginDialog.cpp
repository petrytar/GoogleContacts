#include "LoginDialog.h"
#include "ui_LoginDialog.h"

LoginDialog::LoginDialog(QUrl url, QWidget* parent) :
    BaseClass(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    ui->webView->setUrl(url);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}
