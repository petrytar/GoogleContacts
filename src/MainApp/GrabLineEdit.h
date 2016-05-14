#ifndef MAINAPP_GRABLINEEDIT_H
#define MAINAPP_GRABLINEEDIT_H

#include <QLineEdit>

class GrabLineEdit : public QLineEdit
{
    Q_OBJECT
private:
    typedef QLineEdit BaseClass;

public:
    explicit GrabLineEdit(QWidget* parent = 0);

protected:
    virtual void keyPressEvent(QKeyEvent* event) override;
};

#endif // MAINAPP_GRABLINEEDIT_H
