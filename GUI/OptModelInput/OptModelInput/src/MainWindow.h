#pragma once

#include <QtWidgets/QMainWindow>
#include <QAction>
#include <QFileDialog>
#include <QDomDocument>

#include "ui_MainWindow.h"

#include <iostream>

#include "IsPresentWidget.h"
#include "ModelObjects.h"
#include "ObjectAddDialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    friend class IsPresentWidget;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void onActionOpen();
    void onSaveButtonBoxClicked(QAbstractButton* _button);
    void onModelSave();
    void updateUI();
    void onObjectAdd();
    void onObjectDelete();
    void updateObjectContext();
    void updateContextAndUI();

private:
    void loadModelFromDocument(QDomDocument* _doc);
    void connectContextUpdateSignals();
    void disconnectContextUpdateSignals();

private:
    Ui::MainWindowClass ui;

    IsPresentWidget* m_isPresentWidget;

    QVector<Player*> m_players;
    QVector<Exercise*> m_exercises;

    bool m_wasPlayersView;
    QModelIndex m_previousIndex;
};
