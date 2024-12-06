#pragma once

#include <QDialog>
#include <QWidget>

#include "ui_ObjectAddDialog.h"

class ObjectAddDialog : public QDialog
{
    Q_OBJECT

public:
    ObjectAddDialog(QWidget*parent);
    ~ObjectAddDialog();

    QString getObjectName();
private:

    Ui::ObjectAddDialog ui;
};
