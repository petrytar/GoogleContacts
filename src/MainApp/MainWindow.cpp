#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Data/Auth/AuthManager.h"
#include "Data/Database.h"
#include "Data/GoogleContacts.h"
#include "Data/debugAsserts.h"
#include "Data/Model/ContactEntry.h"
#include "Data/Model/ContactGroup.h"
#include "Data/Model/User.h"
#include "MainApp/ComboBoxDelegate.h"
#include "MainApp/LoginDialog.h"
#include "MainApp/SelectUserDialog.h"
#include "MainApp/EditContactEntryDialog.h"
#include "MainApp/OptionsDialog.h"
#include "MainApp/Settings.h"

#include <QDesktopWidget>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QSplitter>
#include <QTimer>
#include <QFileDialog>

MainWindow::MainWindow(QWidget* parent) :
    BaseClass(parent),
    ui(new Ui::MainWindow),
    m_statusLabel(new QLabel(this)),
    m_statusIconLabel(new QLabel(this)),
    m_settings(new Settings(this)),
    m_networkAccessManager(new QNetworkAccessManager(this)),
    m_authManager(new data::AuthManager(m_networkAccessManager, this)),
    m_database(new data::Database(this)),
    m_googleContacts(new data::GoogleContacts(m_networkAccessManager, m_database, this)),
    m_syncTimer(new QTimer(this))
{
    ui->setupUi(this);
    adjustUi();

    applySettings();

    VERIFY(connect(m_authManager, SIGNAL(newUserInitialiazed(data::ptr<data::User>)), this, SLOT(onNewUserInitialized(data::ptr<data::User>))));
    VERIFY(connect(m_authManager, SIGNAL(error()), this, SLOT(onAuthFailed())));
    VERIFY(connect(m_authManager, SIGNAL(accessTokenReceived(QString)), this, SLOT(onAccessTokenReceived(QString))));
    VERIFY(connect(m_authManager, SIGNAL(invalidRefreshToken()), this, SLOT(onInvalidRefreshToken())));
    VERIFY(connect(m_googleContacts, SIGNAL(groupsSyncSuccessful()), this, SLOT(onGroupsSyncSuccessful())));
    VERIFY(connect(m_googleContacts, SIGNAL(contactsSyncSuccessful()), this, SLOT(onContactsSyncSuccessful())));
    VERIFY(connect(m_googleContacts, SIGNAL(authorizationError()), this, SLOT(onContactsAuthorizationError())));
    VERIFY(connect(m_googleContacts, SIGNAL(otherError(QNetworkReply::NetworkError)), this, SLOT(onContactsOtherError(QNetworkReply::NetworkError))));
    VERIFY(connect(m_syncTimer, SIGNAL(timeout()), this, SLOT(onSyncTimerTimeout())));
}

void MainWindow::adjustUi()
{
    QSplitter* splitter = new QSplitter(this);
    splitter->addWidget(ui->groupsWidget);
    splitter->addWidget(ui->entriesWidget);
    splitter->setSizes(QList<int>() << 100 << 300);
    setCentralWidget(splitter);

    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));

    m_groupRadioButtons.insert(ui->allGroupsRadioButton, data::ptr<data::ContactGroup>());
    VERIFY(connect(ui->allGroupsRadioButton, SIGNAL(toggled(bool)), this, SLOT(onGroupRadioButtonToggled())));

    ui->entriesTreeWidget->setHeaderLabels(QStringList() << "Name" << "Email" << "Phone");
    ui->entriesTreeWidget->setColumnWidth(E_COLUMN_NAME_TO_DISPLAY, 200);
    ui->entriesTreeWidget->setColumnWidth(E_COLUMN_EMAILS, 150);
    ui->entriesTreeWidget->setIndentation(0);

    ui->actionEditContact->setEnabled(false);
    ui->actionDeleteContact->setEnabled(false);

    ui->statusBar->addWidget(m_statusIconLabel);
    ui->statusBar->addWidget(m_statusLabel);
    m_statusIconLabel->setIndent(7);
}

void MainWindow::applySettings()
{
    ui->actionNewContact->setShortcut(QKeySequence(m_settings->getValue(Settings::E_SHORTCUT_NEW_CONTACT)));
    ui->actionEditContact->setShortcut(QKeySequence(m_settings->getValue(Settings::E_SHORTCUT_EDIT_CONTACT)));
    ui->actionDeleteContact->setShortcut(QKeySequence(m_settings->getValue(Settings::E_SHORTCUT_DELETE_CONTACT)));
    ui->actionSynchronize->setShortcut(QKeySequence(m_settings->getValue(Settings::E_SHORTCUT_SYNCHRONIZE)));
    ui->actionExport->setShortcut(QKeySequence(m_settings->getValue(Settings::E_SHORTCUT_EXPORT)));
    ui->actionImport->setShortcut(QKeySequence(m_settings->getValue(Settings::E_SHORTCUT_IMPORT)));
    ui->actionOptions->setShortcut(QKeySequence(m_settings->getValue(Settings::E_SHORTCUT_OPTIONS)));
    ui->actionExit->setShortcut(QKeySequence(m_settings->getValue(Settings::E_SHORTCUT_EXIT)));

    m_syncTimer->setInterval(m_settings->getValue(Settings::E_SYNCHRONIZATION_INTERVAL).toInt() * 1000);
    if (m_syncTimer->isActive())
    {
        m_syncTimer->start();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setup()
{
    m_database->open();

    QList<data::ptr<data::User>> users = m_database->getUsers();
    if (users.empty())
    {
        initNewUser();
    }
    else
    {
        SelectUserDialog* selectUserDialog = new SelectUserDialog(m_database, users, parentWidget());
        VERIFY(connect(selectUserDialog, SIGNAL(newUserRequested()), this, SLOT(initNewUser())));
        VERIFY(connect(selectUserDialog, SIGNAL(userSelected(data::ptr<data::User>)), this, SLOT(setActiveUser(data::ptr<data::User>))));
        selectUserDialog->open();
        qDebug() << "show selectUserDialog";
    }
}

void MainWindow::initNewUser()
{
    m_authManager->start();
    m_loginDialog = new LoginDialog(m_authManager->generateAuthorizationRequestUrl(), parentWidget());
    VERIFY(connect(m_loginDialog, SIGNAL(loadFailed()), this, SLOT(onLoginLoadFailed())));

    m_loginDialog->show();
}

void MainWindow::setActiveUser(data::ptr<data::User> user)
{
    m_googleContacts->setActiveUser(user);
    fillContactGroupsTreeWidget();
    fillContactEntriesTreeWidget();
    m_googleContacts->syncGroupsAndContacts();
    m_syncTimer->start();
    show();
}

void MainWindow::onNewUserInitialized(data::ptr<data::User> user)
{
    if (m_loginDialog)
    {
        m_loginDialog->close();
    }

    m_database->saveOrGetByEmail(user);
    setActiveUser(user);
}

void MainWindow::onLoginLoadFailed()
{
    m_loginDialog->close();
    QMessageBox::critical(this, QString("Error"), QString("Failed to login!"));
    exit(-1);
}

void MainWindow::onAuthFailed()
{
    m_loginDialog->close();
    QMessageBox::critical(this, QString("Error"), QString("Failed authentication!"));
    exit(-1);
}

void MainWindow::onGroupsSyncSuccessful()
{
    fillContactGroupsTreeWidget();
}

void MainWindow::onContactsSyncSuccessful()
{
    fillContactEntriesTreeWidget();
    qDebug() << QString("Successfully syncronized on %1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"));
    updateStatusLabel(true, QString());
}

void MainWindow::onContactsAuthorizationError()
{
    updateStatusLabel(false, QString("Authorization error"));
    qDebug() << QString("Failed to syncronize on %1 (authorization error)").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"));
    m_authManager->refreshAccessToken(m_googleContacts->getActiveUser()->getRefreshToken());
}

void MainWindow::onContactsOtherError(QNetworkReply::NetworkError error)
{
    fillContactEntriesTreeWidget();
    qDebug() << QString("Failed to syncronize on %1 (error code: %2)").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")).arg(error);
    updateStatusLabel(false, QString("Network error"));
}

void MainWindow::onAccessTokenReceived(const QString& accessToken)
{
    m_googleContacts->getActiveUser()->setAccessToken(accessToken);
    m_database->save(m_googleContacts->getActiveUser());
    m_googleContacts->syncGroupsAndContacts();
}

void MainWindow::onInvalidRefreshToken()
{
    updateStatusLabel(false, "Failed to refresh access token (invalid refresh token)");
    qDebug() << "Failed to refresh access token - invalid refresh token";
}

void MainWindow::fillContactGroupsTreeWidget()
{
    QList<data::ptr<data::ContactGroup>> contactGroups = m_googleContacts->getGroups();

    // Remove unnecessary checkboxes

    for (QRadioButton* radioButton : ui->groupsGroupBox->findChildren<QRadioButton*>())
    {
        if (radioButton == ui->allGroupsRadioButton)
        {
            continue;
        }

        data::ptr<data::ContactGroup> contactGroup = m_groupRadioButtons.value(radioButton);
        if (contactGroups.contains(contactGroup))
        {
            continue;
        }

        ui->groupsGroupBoxLayout->removeWidget(radioButton);
        radioButton->deleteLater();
        m_groupRadioButtons.remove(radioButton);
    }

    // Add missing checkboxes

    QList<data::ptr<data::ContactGroup>> displayedContactGroups = m_groupRadioButtons.values();

    for (auto contactGroup : contactGroups)
    {
        if (/*contactGroup->isSystemGroup() || */displayedContactGroups.contains(contactGroup))
        {
            continue;
        }

        QRadioButton* radioButton = new QRadioButton(ui->groupsGroupBox);
        VERIFY(connect(radioButton, SIGNAL(toggled(bool)), this, SLOT(onGroupRadioButtonToggled())));
        radioButton->setText(contactGroup->getTitle());
        ui->groupsGroupBoxLayout->insertWidget(ui->groupsGroupBoxLayout->count() - 1, radioButton);
        m_groupRadioButtons.insert(radioButton, contactGroup);
    }

    // Update texts for existing ones and ensure at least one radio button is checked

    bool hasChecked = false;
    for (auto it = m_groupRadioButtons.begin(); it != m_groupRadioButtons.end(); ++it)
    {
        if (it.key()->isChecked())
        {
            hasChecked = true;
        }

        if (it.key() != ui->allGroupsRadioButton)
        {
            it.key()->setText(it.value()->getTitle());
        }
    }

    if (!hasChecked)
    {
        ui->allGroupsRadioButton->setChecked(true);
    }
}

void MainWindow::onGroupRadioButtonToggled()
{
    QRadioButton* radioButton = qobject_cast<QRadioButton*>(sender());
    if (radioButton->isChecked())
    {
        showFilteredContactEntryItems();
    }
}

void MainWindow::fillContactEntriesTreeWidget()
{
    QList<QTreeWidgetItem*> newAllTreeWidgetItems;

    QList<data::ptr<data::ContactEntry>> contacts = m_googleContacts->getContacts();
    std::set<data::ptr<data::ContactEntry>> newContactEntries;
    for (auto contactEntry : contacts)
    {
        newContactEntries.insert(contactEntry);
    }

    std::set<data::ptr<data::ContactEntry>> displayedContactEntries;
    for (QTreeWidgetItem* item : m_allContactEntryTreeItems)
    {
        data::ptr<data::ContactEntry> contactEntry = item->data(0, Qt::UserRole).value<data::ptr<data::ContactEntry>>();

        if (contactEntry->isDeleted() || newContactEntries.find(contactEntry) == newContactEntries.end())
        {
            ui->entriesTreeWidget->takeTopLevelItem(ui->entriesTreeWidget->indexOfTopLevelItem(item));
            delete item;
        }
        else
        {
            displayedContactEntries.insert(contactEntry);
            updateContactEntryItem(item);
            newAllTreeWidgetItems.push_back(item);
        }
    }

    for (data::ptr<data::ContactEntry> contactEntry : newContactEntries)
    {
        if (!contactEntry->isDeleted() && displayedContactEntries.find(contactEntry) == displayedContactEntries.end())
        {
            QTreeWidgetItem* item = new QTreeWidgetItem();
            item->setData(0, Qt::UserRole, QVariant::fromValue(contactEntry));
            updateContactEntryItem(item);
            newAllTreeWidgetItems.push_back(item);
        }
    }

    m_allContactEntryTreeItems = newAllTreeWidgetItems;
    showFilteredContactEntryItems();
}

void MainWindow::showFilteredContactEntryItems()
{
    QList<QTreeWidgetItem*> contactEntryTreeItems = filterContactEntryItems(m_allContactEntryTreeItems);

    for (int i = ui->entriesTreeWidget->topLevelItemCount() - 1; i >= 0; --i)
    {
        QTreeWidgetItem* item = ui->entriesTreeWidget->topLevelItem(i);
        if (!contactEntryTreeItems.contains(item))
        {
            ui->entriesTreeWidget->takeTopLevelItem(i);
        }
    }

    {
        int i = 0;
        while (i < ui->entriesTreeWidget->topLevelItemCount() && i < contactEntryTreeItems.size())
        {
            QTreeWidgetItem* currentItem = ui->entriesTreeWidget->topLevelItem(i);
            if (currentItem == contactEntryTreeItems[i])
            {
                ++i;
                continue;
            }

            ui->entriesTreeWidget->insertTopLevelItem(i, contactEntryTreeItems[i]);
            ++i;
        }

        while (i < contactEntryTreeItems.size())
        {
            ui->entriesTreeWidget->addTopLevelItem(contactEntryTreeItems[i]);
            ++i;
        }
    }
}

bool MainWindow::itemMatchesFindFilter(QTreeWidgetItem* item)
{
    QRegExp regExp(ui->findLineEdit->text().trimmed());
    regExp.setCaseSensitivity(Qt::CaseInsensitive);
    regExp.setPatternSyntax(QRegExp::Wildcard);
    for (int i = 0; i < item->columnCount(); ++i)
    {
        if (item->text(i).contains(regExp))
        {
            return true;
        }
    }
    return false;
}

QList<QTreeWidgetItem*> MainWindow::filterContactEntryItems(QList<QTreeWidgetItem*> contactEntryTreeItems)
{
    QList<QTreeWidgetItem*> filteredContactEntryItems;

    data::ptr<data::ContactGroup> currentContactGroup;
    for (QRadioButton* radioButton : m_groupRadioButtons.keys())
    {
        if (radioButton->isChecked())
        {
            currentContactGroup = m_groupRadioButtons.value(radioButton);
            break;
        }
    }

    QString findText = ui->findLineEdit->text().trimmed();

    for (QTreeWidgetItem* item : contactEntryTreeItems)
    {
        data::ptr<data::ContactEntry> contactEntry = item->data(0, Qt::UserRole).value<data::ptr<data::ContactEntry>>();
        if (currentContactGroup && !contactEntry->getContactGroups().contains(currentContactGroup))
        {
            continue;
        }

        if (!findText.isEmpty() && !itemMatchesFindFilter(item))
        {
            continue;
        }

        filteredContactEntryItems.push_back(item);
    }
    return filteredContactEntryItems;
}

void MainWindow::updateContactEntryItem(QTreeWidgetItem* item)
{
    data::ptr<data::ContactEntry> contactEntry = item->data(0, Qt::UserRole).value<data::ptr<data::ContactEntry>>();
    item->setText(0, contactEntry->getVisibleName());
    item->setText(1, contactEntry->getPrimaryEmail());
    item->setText(2, contactEntry->getPrimaryPhoneNumber());
}

void MainWindow::setUserContactSingleValueRows(int column, const QStringList& userContactNames)
{
    for (int i = 0; i < userContactNames.size(); ++i)
    {
        //        ui->tableView->setItem(i, column, new QTableWidgetItem(userContactNames.at(i)));
    }
}

void MainWindow::setUserContactListValueRows(int column, const QList<QStringList>& phoneNumbersList)
{
    for (int i = 0; i < phoneNumbersList.size(); ++i)
    {
        for (int j = 0; j < phoneNumbersList.at(i).size(); ++j)
        {
            //            ui->tableView->setItem(i, column, new QTableWidgetItem(userContactNames.at(j)));
        }
    }
}

void MainWindow::synchronizeContacts()
{
    m_googleContacts->syncGroupsAndContacts();
}

void MainWindow::on_actionSynchronize_triggered()
{
    synchronizeContacts();
}

void MainWindow::on_syncButton_clicked()
{
    synchronizeContacts();
}

void MainWindow::editContact()
{
    QTreeWidgetItem* selectedItem = ui->entriesTreeWidget->selectedItems().at(0);
    data::ptr<data::ContactEntry> contactEntry = selectedItem->data(0, Qt::UserRole).value<data::ptr<data::ContactEntry>>();
    EditContactEntryDialog* editContactEntryDialog = new EditContactEntryDialog(contactEntry, m_googleContacts->getGroups(), this);
    auto onAccepted = [this, selectedItem, contactEntry]()
    {
        m_database->update(contactEntry);
        showFilteredContactEntryItems();
    };
    VERIFY(connect(editContactEntryDialog, &QDialog::accepted, onAccepted));
    editContactEntryDialog->open();
}

void MainWindow::on_actionEditContact_triggered()
{
    editContact();
}

void MainWindow::on_editButton_clicked()
{
    editContact();
}

void MainWindow::on_entriesTreeWidget_itemSelectionChanged()
{
    bool hasSelectedItem = !ui->entriesTreeWidget->selectedItems().empty();
    ui->editButton->setEnabled(hasSelectedItem);
    ui->actionEditContact->setEnabled(hasSelectedItem);
    ui->deleteButton->setEnabled(hasSelectedItem);
    ui->actionDeleteContact->setEnabled(hasSelectedItem);
}

void MainWindow::createNewContact()
{
    data::ptr<data::ContactEntry> newContactEntry(new data::ContactEntry());
    EditContactEntryDialog* editContactEntryDialog = new EditContactEntryDialog(newContactEntry, m_googleContacts->getGroups(), this);
    auto onAccepted = [this, newContactEntry]()
    {
        m_googleContacts->addContact(newContactEntry);
        fillContactEntriesTreeWidget();
    };
    VERIFY(connect(editContactEntryDialog, &QDialog::accepted, onAccepted));
    editContactEntryDialog->open();
}

void MainWindow::on_newButton_clicked()
{
    createNewContact();
}

void MainWindow::on_actionNewContact_triggered()
{
    createNewContact();
}

void MainWindow::deleteContact()
{
    QTreeWidgetItem* selectedItem = ui->entriesTreeWidget->selectedItems().at(0);
        int result = QMessageBox::question(this, "Delete contact", QString("Do you want to delete the selected contact entry?"),
            QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
    if (result == QMessageBox::Ok)
    {
        data::ptr<data::ContactEntry> contactEntry = selectedItem->data(0, Qt::UserRole).value<data::ptr<data::ContactEntry>>();
        contactEntry->setDeleted(true);
        m_database->save(contactEntry);
        fillContactEntriesTreeWidget();
    }
}

void MainWindow::on_deleteButton_clicked()
{
    deleteContact();
}

void MainWindow::on_actionDeleteContact_triggered()
{
    deleteContact();
}

void MainWindow::on_findLineEdit_textChanged()
{
    showFilteredContactEntryItems();
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::showOptionsDialog()
{
    OptionsDialog* optionsDialog = new OptionsDialog(m_settings, this);
    auto onAccepted = [this]()
    {
        applySettings();
    };
    VERIFY(connect(optionsDialog, &QDialog::accepted, onAccepted));
    optionsDialog->open();
}

void MainWindow::on_actionOptions_triggered()
{
    showOptionsDialog();
}

void MainWindow::onSyncTimerTimeout()
{
    // Disable synchronization when some dialog is opened
    QList<QDialog*> dialogs = findChildren<QDialog*>();
    for (QDialog* dialog : dialogs)
    {
        if (dialog->isVisible())
        {
            return;
        }
    }
    //qDebug() << "Timer timeout" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    synchronizeContacts();
}

void MainWindow::updateStatusLabel(bool ok, const QString& message)
{
    QString imagePath = ok ? "green_status.png" : "red_status.png";
    m_statusIconLabel->setText(QString("<img src=\":/%1\"/>").arg(imagePath));
    QString statusText = ok ? "Connected" : "Disconnected";
    if (!message.isEmpty())
    {
        statusText.append(" - " + message);
    }
    m_statusLabel->setText(statusText);
}

void MainWindow::exportContacts()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Export Contacts As", "contacts.xml", "XML files (*.xml)");
    if (fileName.isEmpty())
    {
        return;
    }

    if (!m_googleContacts->exportContacts(fileName))
    {
        QMessageBox::warning(this, "Warning", "Export of contacts failed.");
    }
}

void MainWindow::on_actionExport_triggered()
{
    exportContacts();
}

void MainWindow::importContacts()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Import Contacts", QString(), "XML files (*.xml)");
    if (fileName.isEmpty())
    {
        return;
    }

    if (!m_googleContacts->importContacts(fileName))
    {
        QMessageBox::warning(this, "Warning", "Import of contacts failed.");
    }
    else
    {
        fillContactEntriesTreeWidget();
    }
}

void MainWindow::on_actionImport_triggered()
{
    importContacts();
}
