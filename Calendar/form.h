#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include<QMessageBox>
#include "ui_form.h"
#include "todoevent.h"
#include"sqlitestorage.h"

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr);
    void populateEventDetails(const TodoEvent &event);
    ~Form();

    Ui::Form *ui;

private slots:
    void on_checkBox_stateChanged(int state);
    void on_save_clicked();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    bool closedBySaveButton = false; // 标志位，指示是否由保存按钮触发了关闭操作

    // 获取 SQLiteStorage 单例实例
    SQLiteStorage& getSQLiteStorageInstance() {
        return SQLiteStorage::getInstance();
    }
    // 获取 SQLiteStorage 单例实例
    SQLiteStorage& storage = getSQLiteStorageInstance();
};

#endif // FORM_H
