#ifndef DATA_CONTACTENTRY_H
#define DATA_CONTACTENTRY_H

#include <QObject>
#include <QMap>

namespace data
{

/**
 * \brief The ContactEntry class.
 *
 * Class that holds all GoogleContact's entry data
 */
class ContactEntry : public QObject
{
    Q_OBJECT
private:
    typedef QObject BaseClass;

public:
    explicit ContactEntry(QObject* parent);
    virtual ~ContactEntry();

    QString getData(const QString& tagName) const;
    void setData(const QString& tagName, const QString& tagValue);

    // inline implementation
    const QMap<QString, QString>& getData() const { return m_data; }

private:
    QMap<QString, QString> m_data;
};

} // namespace data

#endif // DATA_CONTACTENTRY_H
