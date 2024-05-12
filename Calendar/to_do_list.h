#ifndef TO_DO_LIST_H
#define TO_DO_LIST_H

#include"lunarcalendarwidget.h"

#include <QWidget>
#include <QTableWidgetItem>

namespace Ui {
class To_do_list;
}

class CustomItem : public QTableWidgetItem {
public:
    CustomItem(const QString &text) : QTableWidgetItem(text) {}
    int ID;

};

class To_do_list : public QWidget
{
    Q_OBJECT

public:
    explicit To_do_list(QWidget *parent = nullptr);
    ~To_do_list();

    void showAllEvents();
    void showEventsOnDate(const QDate &date);

private slots:
    void deleteSelectedRows();


    void on_Deleteitem_clicked();

    void on_changeEvent_clicked();

    void on_search_button_clicked();
    void clearHighlightedText();

private:
    Ui::To_do_list *ui;

    void sortTable(int index);
    void sortTableByTime(int Column);
    void sortEventsByPriority();

    // 获取 SQLiteStorage 单例实例
    SQLiteStorage& getSQLiteStorageInstance() {
        return SQLiteStorage::getInstance();
    }

    // 获取 SQLiteStorage 单例实例
    SQLiteStorage& storage = getSQLiteStorageInstance();
};

#endif // TO_DO_LIST_H
