#ifndef SELECTUSERDIALOG_H
#define SELECTUSERDIALOG_H

#include <QDialog>

#include "MainApp/MainApp_global.h"

#include <QMap>

namespace data
{
class User;
class Database;
}

namespace Ui
{
class SelectUserDialog;
}

class SelectUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectUserDialog(data::Database* database, QList<data::ptr<data::User>> users, QWidget* parent);
    ~SelectUserDialog();

signals:
    void newUserRequested();
    void userSelected(data::ptr<data::User> user);

private slots:
    void on_cancelButton_clicked();
    void on_selectButton_clicked();
    void on_newButton_clicked();
    void on_usersTreeWidget_itemSelectionChanged();
    void on_usersTreeWidget_doubleClicked();
    void on_removeButton_clicked();

private:
    Ui::SelectUserDialog* ui;

    data::Database* m_database;
    QMap<QString, data::ptr<data::User>> m_emailsToUsers;
};

#endif // SELECTUSERDIALOG_H
