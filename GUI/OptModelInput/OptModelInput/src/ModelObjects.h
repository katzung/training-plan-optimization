#pragma once
#include <QString>
#include <QVector>

enum TrainingDays
{
    MONDAY,
    WEDNESDAY,
    FRIDAY
};

struct Player
{
public:
    Player(QString _name, int _a, int _b, int _c, QVector<QString> _startTime, QVector<QString> _endTime) :
	m_name(_name),
	m_a(_a),
	m_b(_b),
	m_c(_c),
	m_startTime(_startTime),
	m_endTime(_endTime)
    {
    }
    Player(QString _name) : Player(_name, 34, 33, 33, QVector<QString>{"19:00", "19:00", "19:00"}, QVector<QString>{"21:00", "21:00", "21:00"}) {}

    QString m_name;

    int m_a, m_b, m_c;

    QVector<QString> m_startTime;
    QVector<QString> m_endTime;
};

struct Exercise
{
public:
    Exercise(QString _name, int _aa, int _bb, int _cc, int _duration) :
	m_name(_name),
	m_aa(_aa),
	m_bb(_bb),
	m_cc(_cc),
	m_duration(_duration)
    {
    }
    Exercise(QString _name) : Exercise(_name, 34, 33, 33, 10) {}

    QString m_name;

    int m_aa, m_bb, m_cc;

    int m_duration;
};