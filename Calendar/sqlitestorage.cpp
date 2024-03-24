#include "sqlitestorage.h"

const QString SQLiteStorage::databaseFilename = "todo_events.db";
const QString SQLiteStorage::sqlInit =
    "CREATE TABLE IF NOT EXISTS `TodoEvents` ( "
    "`ID` INTEGER PRIMARY KEY AUTOINCREMENT, "
    "`Name` TEXT, "
    "`StartTime` TEXT, "
    "`EndTime` TEXT, "
    "`Location` TEXT, "
    "`Details` TEXT, "
    "`Priority` TEXT "
    ");";


const QString SQLiteStorage::sqlAddEvent =
    "INSERT INTO `TodoEvents` (`Name`, `StartTime`, `EndTime`, `Location`, `Details`, `Priority`) "
    "VALUES (:name, :start_time, :end_time, :location, :details, :priority);";

const QString SQLiteStorage::sqlGetEventByID =
    "SELECT * FROM `TodoEvents` WHERE `ID` = :id;";

const QString SQLiteStorage::sqlGetAllEvents =
    "SELECT * FROM `TodoEvents`;";

const QString SQLiteStorage::sqlUpdateEvent =
    "UPDATE `TodoEvents` SET `Name` = :name, `StartTime` = :start_time, "
    "`EndTime` = :end_time, `Location` = :location, `Details` = :details, `Priority` = :priority "
    "WHERE `ID` = :id;";


const QString SQLiteStorage::sqlDeleteEvent =
    "DELETE FROM `TodoEvents` WHERE `ID` = :id;";

SQLiteStorage::SQLiteStorage()
    : db(QSqlDatabase::addDatabase("QSQLITE"))
{
    db.setDatabaseName(databaseFilename);
    db.open();
    initializeDatabase();
}

SQLiteStorage::~SQLiteStorage()
{
    db.close();
}

void SQLiteStorage::initializeDatabase()
{
    QSqlQuery query(db);
    if (!query.exec(sqlInit))
    {
        qDebug() << "Error initializing database:" << query.lastError().text();
    }
}

int SQLiteStorage::addEvent(const TodoEvent &event)
{
    QSqlQuery query(db);
    if (!query.prepare(sqlAddEvent))
    {
        qDebug() << "Error preparing SQL statement:" << query.lastError().text();
        return -1;
    }

    query.bindValue(":name", event.name);
    query.bindValue(":start_time", event.startTime);
    query.bindValue(":end_time", event.endTime);
    query.bindValue(":location", event.location);
    query.bindValue(":details", event.details);
    query.bindValue(":priority", event.priority); // 绑定重要程度属性

    if (!query.exec())
    {
        qDebug() << "Error adding event to database:" << query.lastError().text();
        return -1;
    }

    return query.lastInsertId().toInt();
}

TodoEvent SQLiteStorage::getEvent(int id)
{
    QSqlQuery query(db);
    if (!query.prepare(sqlGetEventByID))
    {
        qDebug() << "Error preparing SQL statement:" << query.lastError().text();
        return TodoEvent();
    }

    query.bindValue(":id", id);

    if (!query.exec() || !query.next())
    {
        qDebug() << "Error retrieving event from database:" << query.lastError().text();
        return TodoEvent();
    }

    return TodoEvent(
        query.value("Name").toString(),
        query.value("StartTime").toDateTime(),
        query.value("EndTime").toDateTime(),
        query.value("Location").toString(),
        query.value("Details").toString(),
        query.value("Priority").toString() // 添加对重要程度属性的处理
        );
}

QVector<TodoEvent> SQLiteStorage::getAllEvents()
{
    QVector<TodoEvent> events;

    QSqlQuery query(db);
    if (!query.exec(sqlGetAllEvents))
    {
        qDebug() << "Error retrieving events from database:" << query.lastError().text();
        return events;
    }

    while (query.next())
    {
        events.append(TodoEvent(
            query.value("Name").toString(),
            query.value("StartTime").toDateTime(),
            query.value("EndTime").toDateTime(),
            query.value("Location").toString(),
            query.value("Details").toString(),
            query.value("Priority").toString() // 添加对重要程度属性的处理
            ));
    }

    return events;
}

void SQLiteStorage::updateEvent(int id, const TodoEvent &event)
{
    QSqlQuery query(db);
    if (!query.prepare(sqlUpdateEvent))
    {
        qDebug() << "Error preparing SQL statement:" << query.lastError().text();
        return;
    }

    query.bindValue(":id", id);
    query.bindValue(":name", event.name);
    query.bindValue(":start_time", event.startTime);
    query.bindValue(":end_time", event.endTime);
    query.bindValue(":location", event.location);
    query.bindValue(":details", event.details);
    query.bindValue(":priority", event.priority); // 绑定重要程度属性

    if (!query.exec())
    {
        qDebug() << "Error updating event in database:" << query.lastError().text();
        return;
    }
}

void SQLiteStorage::deleteEvent(int id)
{
    QSqlQuery query(db);
    if (!query.prepare(sqlDeleteEvent))
    {
        qDebug() << "Error preparing SQL statement:" << query.lastError().text();
        return;
    }

    query.bindValue(":id", id);

    if (!query.exec())
    {
        qDebug() << "Error deleting event from database:" << query.lastError().text();
        return;
    }
}
