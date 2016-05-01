#include "EditContactEntryDialog.h"
#include "ui_EditContactEntryDialog.h"

#include "Data/Model/ContactEntry.h"
#include "Data/Model/ContactProperty.h"
#include "Data/Model/ContactGroup.h"
#include "Data/Model/RelValue.h"
#include "MainApp/ContactPropertyComboBoxDelegate.h"

#include <QCheckBox>

EditContactEntryDialog::EditContactEntryDialog(data::ptr<data::ContactEntry> contactEntry, QList<data::ptr<data::ContactGroup>> contactGroups, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::EditContactEntryDialog),
    m_contactEntry(contactEntry),
    m_contactGroups(contactGroups)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    VERIFY(connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject())));
    VERIFY(connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(onSaveButtonClicked())));

    ui->nameEdit->setText(contactEntry->getName());
    ui->nicknameEdit->setText(contactEntry->getNickname());
    ui->fileAsEdit->setText(contactEntry->getFileAs());
    ui->companyEdit->setText(contactEntry->getOrgName());
    ui->jobTitleEdit->setText(contactEntry->getOrgTitle());

    for (auto contactGroup : contactGroups)
    {
        /*if (contactGroup->isSystemGroup())
        {
            continue;
        }*/
        QCheckBox* checkBox = new QCheckBox(ui->groupsGroupBox);
        checkBox->setText(contactGroup->getTitle());
        ui->groupsGroupBoxLayout->insertWidget(ui->groupsGroupBoxLayout->count() - 1, checkBox);
        checkBox->setChecked(contactEntry->getContactGroups().contains(contactGroup));
        m_checkBoxesToContactGroups.insert(checkBox, contactGroup);
    }

    initPropertiesTreeWidget();
}

EditContactEntryDialog::~EditContactEntryDialog()
{
    delete ui;
}

void EditContactEntryDialog::initPropertiesTreeWidget()
{
    ui->propertiesTreeWidget->setColumnWidth(0, 150);
    fillContactProperties("Emails", m_contactEntry->getEmails());
    fillContactProperties("Phones", m_contactEntry->getPhoneNumbers());
    ui->propertiesTreeWidget->expandAll();

    ui->propertiesTreeWidget->setItemDelegateForColumn(0, new ContactPropertyComboBoxDelegate(this));
}

void EditContactEntryDialog::fillContactProperties(const QString& categoryName, QList<data::ptr<data::ContactProperty>> properties)
{
    QTreeWidgetItem* parentItem = new QTreeWidgetItem(ui->propertiesTreeWidget, QStringList() << categoryName);
    for (data::ptr<data::ContactProperty> property : properties)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(parentItem);
        initPropertiesTreeWidgetItem(item, property);
    }
}

void EditContactEntryDialog::initPropertiesTreeWidgetItem(QTreeWidgetItem* item, data::ptr<data::ContactProperty> property)
{
    QString labelForRelValue = data::RelValue::getLabelFromUrl(property->getLabel());
    QString label = labelForRelValue.isEmpty() ? property->getLabel() : labelForRelValue;
    item->setText(0, label);
    item->setText(1, property->getValue());
    item->setData(0, Qt::UserRole, QVariant::fromValue(property));
    item->setFlags(item->flags() | Qt::ItemIsEditable);
}

void EditContactEntryDialog::onSaveButtonClicked()
{
    m_contactEntry->setName(ui->nameEdit->text());
    m_contactEntry->setNickname(ui->nicknameEdit->text());
    m_contactEntry->setFileAs(ui->fileAsEdit->text());
    m_contactEntry->setOrgName(ui->companyEdit->text());
    m_contactEntry->setOrgTitle(ui->jobTitleEdit->text());
    m_contactEntry->setUpdatedTime(QDateTime::currentDateTime());

    QList<data::ptr<data::ContactProperty>> properties;

    for (int i = 0; i < ui->propertiesTreeWidget->topLevelItemCount(); ++i)
    {
        auto topLevelItem = ui->propertiesTreeWidget->topLevelItem(i);
        for (int j = 0; j < topLevelItem->childCount(); ++j)
        {
            auto item = topLevelItem->child(j);
            data::ptr<data::ContactProperty> property = item->data(0, Qt::UserRole).value<data::ptr<data::ContactProperty>>();
            QString urlForRelValue = data::RelValue::getUrlFromLabel(item->text(0).trimmed());
            QString label = urlForRelValue.isEmpty() ? item->text(0).trimmed() : urlForRelValue;
            property->setLabel(label);
            property->setValue(item->text(1).trimmed());
            properties.append(property);
        }
    }
    m_contactEntry->setProperties(properties);

    QList<data::ptr<data::ContactGroup>> groups;
    /*for (auto contactGroup : m_contactGroups)
    {
        if (contactGroup->getGoogleShortId() == "6")
        {
            groups.append(contactGroup);
            break;
        }
    }*/
    for (int i = 0; i < ui->groupsGroupBoxLayout->count() - 1; ++i)
    {
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(ui->groupsGroupBoxLayout->itemAt(i)->widget());
        if (checkBox->isChecked())
        {
            groups.append(m_checkBoxesToContactGroups.value(checkBox));
        }
    }
    m_contactEntry->setContactGroups(groups);

    accept();
}


void EditContactEntryDialog::on_propertiesTreeWidget_itemSelectionChanged()
{
    ui->addButton->setEnabled(!ui->propertiesTreeWidget->selectedItems().empty());
    ui->deleteButton->setEnabled(!ui->propertiesTreeWidget->selectedItems().empty() && ui->propertiesTreeWidget->selectedItems().at(0)->parent());
}

void EditContactEntryDialog::on_addButton_clicked()
{
    QTreeWidgetItem* item = ui->propertiesTreeWidget->selectedItems().at(0);
    QTreeWidgetItem* topLevelItem = item->parent() ? item->parent() : item;
    data::ContactProperty::EType type = ui->propertiesTreeWidget->indexOfTopLevelItem(topLevelItem) == 0
            ? data::ContactProperty::E_TYPE_EMAIL
            : data::ContactProperty::E_TYPE_PHONE_NUMBER;
    data::ptr<data::ContactProperty> contactProperty(new data::ContactProperty(m_contactEntry, QString(), "<new>", type));

    QTreeWidgetItem* newItem = new QTreeWidgetItem();
    initPropertiesTreeWidgetItem(newItem, contactProperty);
    if (item == topLevelItem)
    {
        topLevelItem->addChild(newItem);
    }
    else
    {
        topLevelItem->insertChild(topLevelItem->indexOfChild(item) + 1, newItem);
    }
}

void EditContactEntryDialog::on_deleteButton_clicked()
{
    QTreeWidgetItem* item = ui->propertiesTreeWidget->selectedItems().at(0);
    if (item->parent())
    {
        item->parent()->removeChild(item);
    }
    else
    {
        ui->propertiesTreeWidget->takeTopLevelItem(ui->propertiesTreeWidget->indexOfTopLevelItem(item));
    }
    delete item;
}
