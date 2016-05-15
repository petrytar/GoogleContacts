#include "Settings.h"

#include <QSettings>

Settings::Settings(QObject* parent):
    BaseClass(parent)
{
    m_settings = new QSettings("settings.ini", QSettings::IniFormat);
}

QString Settings::getPropertyKey(EProperty property)
{
    switch (property)
    {
        case E_SHORTCUT_NEW_CONTACT:
        {
            return "E_SHORTCUT_NEW_CONTACT";
        }
        case E_SHORTCUT_EDIT_CONTACT:
        {
            return "E_SHORTCUT_EDIT_CONTACT";
        }
        case E_SHORTCUT_DELETE_CONTACT:
        {
            return "E_SHORTCUT_DELETE_CONTACT";
        }
        case E_SHORTCUT_SYNCHRONIZE:
        {
            return "E_SHORTCUT_SYNCHRONIZE";
        }
        case E_SHORTCUT_EXPORT:
        {
            return "E_SHORTCUT_EXPORT";
        }
        case E_SHORTCUT_IMPORT:
        {
            return "E_SHORTCUT_IMPORT";
        }
        case E_SHORTCUT_OPTIONS:
        {
            return "E_SHORTCUT_OPTIONS";
        }
        case E_SHORTCUT_EXIT:
        {
            return "E_SHORTCUT_EXIT";
        }
        case E_SYNCHRONIZATION_INTERVAL:
        {
            return "E_SYNCHRONIZATION_INTERVAL";
        }
    }

    return QString();
}

QString Settings::getDefaultValue(EProperty property)
{
    switch (property)
    {
        case E_SHORTCUT_NEW_CONTACT:
        {
            return "Ctrl+N";
        }
        case E_SHORTCUT_EDIT_CONTACT:
        {
            return "Ctrl+E";
        }
        case E_SHORTCUT_DELETE_CONTACT:
        {
            return "Del";
        }
        case E_SHORTCUT_SYNCHRONIZE:
        {
            return "Ctrl+S";
        }
        case E_SHORTCUT_EXPORT:
        {
            return "Ctrl+X";
        }
        case E_SHORTCUT_IMPORT:
        {
            return "Ctrl+I";
        }
        case E_SHORTCUT_OPTIONS:
        {
            return "Ctrl+O";
        }
        case E_SHORTCUT_EXIT:
        {
            return "Ctrl+Q";
        }
        case E_SYNCHRONIZATION_INTERVAL:
        {
            return "15";
        }
    }

    return QString();
}

QString Settings::getValue(EProperty property)
{
    QString key = getPropertyKey(property);
    if (key.isEmpty())
    {
        return QString();
    }
    return m_settings->value(key, getDefaultValue(property)).toString();
}

void Settings::setValue(EProperty property, const QString& value)
{
    QString key = getPropertyKey(property);
    if (key.isEmpty())
    {
        return;
    }
    m_settings->setValue(key, value);
}
