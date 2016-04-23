#include "SelectUserDialog.h"
#include "ui_SelectUserDialog.h"

#include "Data/Model/User.h"
#include "Data/Database.h"

#include <QMessageBox>

SelectUserDialog::SelectUserDialog(data::Database* database, QList<data::ptr<data::User>> users, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::SelectUserDialog),
    m_database(database)
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
    bool hasSelectedItem = !ui->usersTreeWidget->selectedItems().empty();
    ui->selectButton->setEnabled(hasSelectedItem);
    ui->removeButton->setEnabled(hasSelectedItem);
}

void SelectUserDialog::on_usersTreeWidget_doubleClicked()
{
    on_selectButton_clicked();
}

void SelectUserDialog::on_removeButton_clicked()
{
    QTreeWidgetItem* item = ui->usersTreeWidget->selectedItems().at(0);
    QString selectedEmail = item->text(0);
    int result = QMessageBox::question(this, "Remove account", QString("Do you want to remove all data associated with the following Google account: %1?").arg(selectedEmail),
            QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
    if (result == QMessageBox::Ok)
    {
        data::ptr<data::User> user = m_emailsToUsers.value(selectedEmail);
        m_database->remove(user);
        ui->usersTreeWidget->takeTopLevelItem(ui->usersTreeWidget->indexOfTopLevelItem(item));
    }
}
