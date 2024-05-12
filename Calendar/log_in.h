#ifndef LOG_IN_H
#define LOG_IN_H

#include <QWidget>
#include<QSqlDatabase>


namespace Ui {
class log_in;
}

class log_in : public QWidget
{
    Q_OBJECT

public:
    log_in(QWidget *parent = nullptr);
    ~log_in();
    static int getDatabaseErrorCount(); // 静态成员函数，用于获取数据库查询错误次数

private slots:
    void on_newpushButton_clicked();     // 注册新用户按钮点击事件
    void createTable();                  // 创建数据库表
    void on_loadpushButton_clicked();    // 登录按钮点击事件
    void on_showcodepushButton_clicked();// 显示/隐藏密码按钮点击事件
    void on_namecomboBox_activated(int index); // 当用户名下拉框激活时
    void loadUsernamesFromDatabase();    // 从数据库加载用户名

private:
    Ui::log_in *ui;
    QSqlDatabase db;
    bool passwordVisible;
};

#endif // LOG_IN_H
