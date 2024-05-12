#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include<QSqlDatabase>

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();



private slots:
    void on_namecomboBox_activated(int index);

    void on_showcodepushButton_clicked();

    void on_loadpushButton_clicked();

    void on_newpushButton_clicked();

    void loadUsernamesFromDatabase();

private:
    Ui::Login *ui;
    void sqlite_Init();
    bool passwordVisible;

    QSqlDatabase login_db;
};

#endif // LOGIN_H
