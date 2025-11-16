#include "ObjectAddDialog.h"

ObjectAddDialog::ObjectAddDialog(QWidget *parent)
	: QDialog(parent)
{
    ui.setupUi(this);
}

ObjectAddDialog::~ObjectAddDialog()
{}

QString ObjectAddDialog::getObjectName()
{
    return ui.objectNameLineEdit->text();
}
