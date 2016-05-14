#ifndef MAINAPP_SETTINGS_H
#define MAINAPP_SETTINGS_H

#include <QObject>

class QSettings;

class Settings : public QObject
{
    Q_OBJECT
private:
    typedef QObject BaseClass;

public:
    explicit Settings(QObject* parent);

    enum EProperty
    {
        E_SHORTCUT_NEW_CONTACT,
        E_SHORTCUT_EDIT_CONTACT,
        E_SHORTCUT_DELETE_CONTACT,
        E_SHORTCUT_SYNCHRONIZE,
        E_SHORTCUT_OPTIONS,
        E_SHORTCUT_EXIT
    };

    QString getValue(EProperty property);
    QString getDefaultValue(EProperty property);
    void setValue(EProperty property, const QString& value);

private:
    QString getPropertyKey(EProperty property);

    QSettings* m_settings;
};

#endif // MAINAPP_SETTINGS_H
