#pragma once

#include <QWidget>

#include "ui_IsPresentWidget.h"

#include "ModelObjects.h"
#include "MainWindow.h"

class IsPresentWidget : public QWidget
{
    Q_OBJECT

    friend class MainWindow;
public:
    IsPresentWidget(QWidget* parent = nullptr);
    ~IsPresentWidget();

private:
    void updateUI(Player* _player);
    void updatePlayer(Player* _player);

    void connectContextUpdateSignals(MainWindow* _mainWindow);
    void disconnectContextUpdateSignals(MainWindow* _mainWindow);

    Ui::IsPresentWidgetClass ui;
};

