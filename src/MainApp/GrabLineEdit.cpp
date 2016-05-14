#include "GrabLineEdit.h"

#include <QKeyEvent>

GrabLineEdit::GrabLineEdit(QWidget* parent) :
    BaseClass(parent)
{
    setPlaceholderText("Type to set shortcut");
}

void GrabLineEdit::keyPressEvent(QKeyEvent* event)
{
    int key = event->key();
    if (key == Qt::Key_Shift || key == Qt::Key_Control || key == Qt::Key_Alt)
    {
        key = 0;
    }
    int modifiers = event->modifiers();
    if (modifiers & Qt::ControlModifier)
    {
        key += Qt::CTRL;
    }
    if (modifiers & Qt::ShiftModifier)
    {
        key += Qt::SHIFT;
    }
    if (modifiers & Qt::AltModifier)
    {
        key += Qt::ALT;
    }

    QKeySequence sequence(key);
    setText(sequence.toString());

    //QLineEdit::keyPressEvent(event); // disable default processing
}
