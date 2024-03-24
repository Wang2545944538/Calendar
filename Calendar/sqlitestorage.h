#ifndef SQLITESTORAGE_H
#define SQLITESTORAGE_H

#include "todoevent.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>

class SQLiteStorage
{
public:
    SQLiteStorage();
    ~SQLiteStorage();

    int addEvent(const TodoEvent &event);
    TodoEvent getEvent(int id);
    QVector<TodoEvent> getAllEvents();
    void updateEvent(int id, const TodoEvent &event);
    void deleteEvent(int id);

private:
    QSqlDatabase db;

    static const QString databaseFilename;
    static const QString sqlInit;
    static const QString sqlAddEvent;
    static const QString sqlGetEventByID;
    static const QString sqlGetAllEvents;
    static const QString sqlUpdateEvent;
    static const QString sqlDeleteEvent;

    void initializeDatabase();
};

#endif // SQLITESTORAGE_H
