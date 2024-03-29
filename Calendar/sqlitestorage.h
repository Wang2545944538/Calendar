#ifndef SQLITESTORAGE_H
#define SQLITESTORAGE_H

#include "todoevent.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVector>
#include <QDate>
#include <QSqlError>

class SQLiteStorage
{
public:
    static SQLiteStorage& getInstance();
    ~SQLiteStorage();

    int addEvent(const TodoEvent &event);
    TodoEvent getEvent(int id);
    QVector<TodoEvent> getAllEvents();
    void updateEvent(int id, const TodoEvent &event);
    void deleteEvent(int id);
    bool hasEventOnDate(const QDate &date);
    QVector<QPair<int, TodoEvent>> getAllEventsWithIDs();

private:
    SQLiteStorage(); // Private constructor
    SQLiteStorage(const SQLiteStorage&) = delete; // Disable copy constructor
    SQLiteStorage& operator=(const SQLiteStorage&) = delete; // Disable assignment operator

    QSqlDatabase db;

    static const QString databaseFilename;
    static const QString sqlInit;
    static const QString sqlAddEvent;
    static const QString sqlGetEventByID;
    static const QString sqlGetAllEvents;
    static const QString sqlUpdateEvent;
    static const QString sqlDeleteEvent;
    static const QString sqlGetEventsOnDate;

    void initializeDatabase();
};

#endif // SQLITESTORAGE_H
