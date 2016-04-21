#ifndef MAINAPP_CONTACTPROPERTYCOMBOBOXDELEGATE_H
#define MAINAPP_CONTACTPROPERTYCOMBOBOXDELEGATE_H

#include <QStyledItemDelegate>

class ContactPropertyComboBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
private:
    typedef QStyledItemDelegate BaseClass;

public:
    explicit ContactPropertyComboBoxDelegate(QObject* parent = 0);
    virtual ~ContactPropertyComboBoxDelegate();

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    void setEditorData(QWidget* editor, const QModelIndex& index) const;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};


#endif // MAINAPP_CONTACTPROPERTYCOMBOBOXDELEGATE_H
