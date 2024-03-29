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

const QString SQLiteStorage::sqlGetEventsOnDate =
    "SELECT * FROM `TodoEvents` WHERE `StartTime` <= :end_time AND `EndTime` >= :start_time;";

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

SQLiteStorage::SQLiteStorage() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(databaseFilename);
    if (!db.open()) {
        qDebug() << "Error opening database:" << db.lastError().text();
    }
    initializeDatabase();
}

SQLiteStorage::~SQLiteStorage() {
    db.close();
}

SQLiteStorage& SQLiteStorage::getInstance() {
    static SQLiteStorage instance;
    return instance;
}

void SQLiteStorage::initializeDatabase() {
    QSqlQuery query(db);
    if (!query.exec(sqlInit)) {
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
        TodoEvent event(
            query.value("Name").toString(),
            query.value("StartTime").toDateTime(),
            query.value("EndTime").toDateTime(),
            query.value("Location").toString(),
            query.value("Details").toString(),
            query.value("Priority").toString()

            );
        events.append(event);
    }

    return events;
}

QVector<QPair<int, TodoEvent>> SQLiteStorage::getAllEventsWithIDs()
{
    QVector<QPair<int, TodoEvent>> eventsWithIDs;

    QSqlQuery query(db);
    if (!query.exec(sqlGetAllEvents))
    {
        qDebug() << "Error retrieving events from database:" << query.lastError().text();
        return eventsWithIDs;
    }

    while (query.next())
    {
        int id = query.value("ID").toInt(); // 获取ID属性并转换为整数
        TodoEvent event(
            query.value("Name").toString(),
            query.value("StartTime").toDateTime(),
            query.value("EndTime").toDateTime(),
            query.value("Location").toString(),
            query.value("Details").toString(),
            query.value("Priority").toString()
            );
        eventsWithIDs.append(qMakePair(id, event));
    }

    return eventsWithIDs;
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

bool SQLiteStorage::hasEventOnDate(const QDate &date)
{
    // 构建日期范围，从所提供的日期的开始到结束
    QDateTime startOfDay(QDate(date.year(), date.month(), date.day()), QTime(0, 0, 0)); // 设置时间为当天的开始
    QDateTime endOfDay = startOfDay;
    endOfDay.setTime(QTime(23, 59, 59)); // 设置时间为当天的最后一秒

    QSqlQuery query(db);
    if (!query.prepare(sqlGetEventsOnDate)) // 正确使用了 sqlGetEventsOnDate
    {
        qDebug() << "Error preparing SQL statement:" << query.lastError().text();
        return false;
    }

    query.prepare(sqlGetEventsOnDate);
    query.bindValue(":start_time", startOfDay.toString(Qt::ISODate));
    query.bindValue(":end_time", endOfDay.toString(Qt::ISODate));

    if (!query.exec())
    {
        qDebug() << "Error executing SQL query:" << query.lastError().text();
        return false;
    }

    if (!query.next())
    {
        qDebug() << "No events found for date:" << date.toString(Qt::ISODate);
        return false;
    }

    int count = query.value(0).toInt();
    return count > 0; // 如果事件数大于0，则表示有事件发生在该日期
}
