#include "EditContactEntryDialog.h"
#include "ui_EditContactEntryDialog.h"

#include "Data/Model/ContactEntry.h"
#include "Data/Model/ContactProperty.h"

EditContactEntryDialog::EditContactEntryDialog(data::ptr<data::ContactEntry> contactEntry, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::EditContactEntryDialog),
    m_contactEntry(contactEntry)
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
}

void EditContactEntryDialog::fillContactProperties(const QString& categoryName, QList<data::ptr<data::ContactProperty>> properties)
{
    QTreeWidgetItem* parentItem = new QTreeWidgetItem(ui->propertiesTreeWidget, QStringList() << categoryName);
    for (data::ptr<data::ContactProperty> property : properties)
    {
        new QTreeWidgetItem(parentItem, QStringList() << property->getName() << property->getValue());
    }
}

void EditContactEntryDialog::onSaveButtonClicked()
{
    m_contactEntry->setName(ui->nameEdit->text());
    m_contactEntry->setNickname(ui->nicknameEdit->text());
    m_contactEntry->setFileAs(ui->fileAsEdit->text());
    m_contactEntry->setOrgName(ui->companyEdit->text());
    m_contactEntry->setOrgTitle(ui->jobTitleEdit->text());
    m_contactEntry->setUpdatedTime(QDateTime::currentDateTime());
    accept();
}
