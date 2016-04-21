#include "ContactPropertyComboBoxDelegate.h"

#include "Data/Model/ContactProperty.h"
#include "Data/Model/RelValue.h"

#include <QComboBox>

ContactPropertyComboBoxDelegate::ContactPropertyComboBoxDelegate(QObject* parent) :
    BaseClass(parent)
{
}

ContactPropertyComboBoxDelegate::~ContactPropertyComboBoxDelegate()
{
}

QWidget* ContactPropertyComboBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QComboBox* comboBox = new QComboBox(parent);
    comboBox->setEditable(true);
    data::ptr<data::ContactProperty> property = index.data(Qt::UserRole).value<data::ptr<data::ContactProperty>>();
    QList<data::RelValue::ERelValue> relValues;
    switch (property->getType())
    {
        case data::ContactProperty::E_TYPE_EMAIL:
        {
            relValues = data::RelValue::getEmailRelValues();
            break;
        }
        case data::ContactProperty::E_TYPE_PHONE_NUMBER:
        {
            relValues = data::RelValue::getPhoneNumberRelValues();
            break;
        }
        default:
        {
            break;
        }
    }

    for (auto relValue : relValues)
    {
        comboBox->addItem(data::RelValue::getLabel(relValue));
    }
    return comboBox;
}

void ContactPropertyComboBoxDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QComboBox* comboBox = static_cast<QComboBox*>(editor);
    int itemIndex = comboBox->findText(index.data().toString());
    if (itemIndex >= 0)
    {
        comboBox->setCurrentIndex(itemIndex);
    }
    else
    {
        comboBox->setEditText(index.data().toString());
    }
}

void ContactPropertyComboBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QComboBox* comboBox = static_cast<QComboBox*>(editor);
    model->setData(index, comboBox->currentText().trimmed(), Qt::EditRole);
}

void ContactPropertyComboBoxDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& /* index */) const
{
    editor->setGeometry(option.rect);
}
