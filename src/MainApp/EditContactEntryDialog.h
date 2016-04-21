#ifndef EDITCONTACTENTRYDIALOG_H
#define EDITCONTACTENTRYDIALOG_H

#include <QDialog>

#include "MainApp/MainApp_global.h"

#include <QList>

class QTreeWidgetItem;

namespace data
{
class ContactEntry;
class ContactProperty;
}

namespace Ui
{
class EditContactEntryDialog;
}

class EditContactEntryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditContactEntryDialog(data::ptr<data::ContactEntry> contactEntry, QWidget* parent);
    ~EditContactEntryDialog();

private slots:
    void onSaveButtonClicked();

    void on_propertiesTreeWidget_itemSelectionChanged();

    void on_addButton_clicked();

    void on_deleteButton_clicked();

private:
    void initPropertiesTreeWidget();
    void fillContactProperties(const QString& categoryName, QList<data::ptr<data::ContactProperty>> properties);
    void initPropertiesTreeWidgetItem(QTreeWidgetItem* item, data::ptr<data::ContactProperty> contactProperty);

    Ui::EditContactEntryDialog* ui;

    data::ptr<data::ContactEntry> m_contactEntry;
};

#endif // EDITCONTACTENTRYDIALOG_H
