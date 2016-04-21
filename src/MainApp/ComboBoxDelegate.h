#ifndef MAINAPP_COMBOBOXDELEGATE_H
#define MAINAPP_COMBOBOXDELEGATE_H

#include <QItemDelegate>

/**
 * \brief The ComboBoxDelegate class.
 *
 * Add some description here.
 */
class ComboBoxDelegate : public QItemDelegate
{
    Q_OBJECT
private:
    typedef QItemDelegate BaseClass;

public:
    explicit ComboBoxDelegate(const QStringList& items, QObject* parent = 0);
    virtual ~ComboBoxDelegate();

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    void setEditorData(QWidget* editor, const QModelIndex& index) const;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    //void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
    QList<QString> m_items;
};

#endif // MAINAPP_COMBOBOXDELEGATE_H
