#include "ComboBoxDelegate.h"

#include <QComboBox>
#include <QApplication>
#include <QString>

#include <iostream>

ComboBoxDelegate::ComboBoxDelegate(const QStringList& items, QObject* parent) :
    BaseClass(parent),
    m_items(items)
{
}

ComboBoxDelegate::~ComboBoxDelegate()
{
}

QWidget* ComboBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& /* option */, const QModelIndex& /* index */) const
{
    QComboBox* comboBox = new QComboBox(parent);
    for (QString item : m_items)
    {
        comboBox->addItem(item);
    }
    return comboBox;
}

void ComboBoxDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QComboBox* comboBox = static_cast<QComboBox*>(editor);
    int value = index.model()->data(index, Qt::EditRole).toUInt();
    comboBox->setCurrentIndex(value);
}

void ComboBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QComboBox* comboBox = static_cast<QComboBox*>(editor);
    model->setData(index, comboBox->currentIndex(), Qt::EditRole);
}

void ComboBoxDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& /* index */) const
{
    editor->setGeometry(option.rect);
}

/*void ComboBoxDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyleOptionViewItemV4 myOption = option;
    QString text = m_items.at(index.row());

    myOption.text = text;

    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &myOption, painter);
}*/
