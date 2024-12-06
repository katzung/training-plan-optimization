#include "IsPresentWidget.h"

IsPresentWidget::IsPresentWidget(QWidget* parent) 
    : QWidget(parent)
{
    ui.setupUi(this);

    for (QTimeEdit* timeEdit : this->findChildren<QTimeEdit*>()) {
	timeEdit->setTimeZone(QTimeZone::systemTimeZone());
	timeEdit->setDisplayFormat("h:mm");
	timeEdit->setDateTime(QDateTime::fromString("19:00", "hh:mm"));
	timeEdit->setMinimumDateTime(QDateTime::fromString("19:00", "hh:mm"));
	timeEdit->setMaximumDateTime(QDateTime::fromString("21:00", "hh:mm"));
    }
}

IsPresentWidget::~IsPresentWidget()
{
}

void IsPresentWidget::updateUI(Player* _player)
{
    ui.timeEdit->setTime(QTime::fromString(_player->m_startTime[MONDAY], "hh:mm"));
    ui.timeEdit_2->setTime(QTime::fromString(_player->m_endTime[MONDAY], "hh:mm"));
    ui.timeEdit_3->setTime(QTime::fromString(_player->m_startTime[WEDNESDAY], "hh:mm"));
    ui.timeEdit_4->setTime(QTime::fromString(_player->m_endTime[WEDNESDAY], "hh:mm"));
    ui.timeEdit_5->setTime(QTime::fromString(_player->m_startTime[FRIDAY], "hh:mm"));
    ui.timeEdit_6->setTime(QTime::fromString(_player->m_endTime[FRIDAY], "hh:mm"));
}

void IsPresentWidget::updatePlayer(Player* _player)
{
    _player->m_startTime[MONDAY] = ui.timeEdit->time().toString("hh:mm");
    _player->m_endTime[MONDAY] = ui.timeEdit_2->time().toString("hh:mm");
    _player->m_startTime[WEDNESDAY] = ui.timeEdit_3->time().toString("hh:mm");
    _player->m_endTime[WEDNESDAY] = ui.timeEdit_4->time().toString("hh:mm");
    _player->m_startTime[FRIDAY] = ui.timeEdit_5->time().toString("hh:mm");
    _player->m_endTime[FRIDAY] = ui.timeEdit_6->time().toString("hh:mm");
}

void IsPresentWidget::connectContextUpdateSignals(MainWindow* _mainWindow)
{
    connect(ui.timeEdit, SIGNAL(timeChanged(QTime)), _mainWindow, SLOT(updateObjectContext()));
    connect(ui.timeEdit_2, SIGNAL(timeChanged(QTime)), _mainWindow, SLOT(updateObjectContext()));
    connect(ui.timeEdit_3, SIGNAL(timeChanged(QTime)), _mainWindow, SLOT(updateObjectContext()));
    connect(ui.timeEdit_4, SIGNAL(timeChanged(QTime)), _mainWindow, SLOT(updateObjectContext()));
    connect(ui.timeEdit_5, SIGNAL(timeChanged(QTime)), _mainWindow, SLOT(updateObjectContext()));
    connect(ui.timeEdit_6, SIGNAL(timeChanged(QTime)), _mainWindow, SLOT(updateObjectContext()));
}

void IsPresentWidget::disconnectContextUpdateSignals(MainWindow* _mainWindow)
{
    disconnect(ui.timeEdit, SIGNAL(timeChanged(QTime)), _mainWindow, SLOT(updateObjectContext()));
    disconnect(ui.timeEdit_2, SIGNAL(timeChanged(QTime)), _mainWindow, SLOT(updateObjectContext()));
    disconnect(ui.timeEdit_3, SIGNAL(timeChanged(QTime)), _mainWindow, SLOT(updateObjectContext()));
    disconnect(ui.timeEdit_4, SIGNAL(timeChanged(QTime)), _mainWindow, SLOT(updateObjectContext()));
    disconnect(ui.timeEdit_5, SIGNAL(timeChanged(QTime)), _mainWindow, SLOT(updateObjectContext()));
    disconnect(ui.timeEdit_6, SIGNAL(timeChanged(QTime)), _mainWindow, SLOT(updateObjectContext()));
}
