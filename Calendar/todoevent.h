#ifndef TODOEVENT_H
#define TODOEVENT_H

#include <QString>
#include <QDateTime>

class TodoEvent
{
public:
    QString name;
    QDateTime startTime;
    QDateTime endTime;
    QString location;
    QString details;
    QString priority;


    TodoEvent(){}
    TodoEvent(const QString &name, const QDateTime &startTime, const QDateTime &endTime, const QString &location, const QString &details, const QString &priority)
        : name(name), startTime(startTime), endTime(endTime), location(location), details(details), priority(priority) {}


};

#endif // TODOEVENT_H
