#include "SelectUserDialog.h"
#include "ui_SelectUserDialog.h"

#include "Data/Model/User.h"

SelectUserDialog::SelectUserDialog(QList<data::ptr<data::User>> users, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::SelectUserDialog)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    for (data::ptr<data::User> user : users)
    {
        m_emailsToUsers.insert(user->getEmail(), user);
        new QTreeWidgetItem(ui->usersTreeWidget, QStringList() << user->getEmail());
    }

    if (!users.empty())
    {
        ui->usersTreeWidget->topLevelItem(0)->setSelected(true);
    }
}

SelectUserDialog::~SelectUserDialog()
{
    delete ui;
}

void SelectUserDialog::on_cancelButton_clicked()
{
    reject();
}

void SelectUserDialog::on_selectButton_clicked()
{
    accept();
    QString selectedEmail = ui->usersTreeWidget->selectedItems().at(0)->text(0);
    emit userSelected(m_emailsToUsers.value(selectedEmail));
}

void SelectUserDialog::on_newButton_clicked()
{
    accept();
    emit newUserRequested();
}

void SelectUserDialog::on_usersTreeWidget_itemSelectionChanged()
{
    ui->selectButton->setEnabled(!ui->usersTreeWidget->selectedItems().empty());
}

void SelectUserDialog::on_usersTreeWidget_doubleClicked()
{
    on_selectButton_clicked();
}
