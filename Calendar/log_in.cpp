#include "log_in.h"
#include "ui_log_in.h"
#include"lunarcalendarwidget.h"

#include <QSqlQuery>
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>

log_in::log_in(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::log_in)
{
    ui->setupUi(this);
    this->setWindowTitle("登录");

    // 创建 QLabel 并设置背景图片
    QLabel *backgroundLabel = new QLabel(this);
    QPixmap backgroundImage(":/resources/image/login.png");
    backgroundLabel->setPixmap(backgroundImage);
    backgroundLabel->setScaledContents(true);  // 缩放图片以适应 QLabel 的大小
    backgroundLabel->setGeometry(0, 0, this->width(), this->height());
    backgroundLabel->lower();  // 将背景图片放置在底层

    /***建立数据库连接***/
    this->db=QSqlDatabase::addDatabase ("QSQLITE");
    db.setDatabaseName ("first.db");

    bool ok;
    ok=db.open ();
    if(!ok)
    {
        QMessageBox::information (this,"错误","数据库打开失败");
        qDebug()<<"数据库打开失败";
        qDebug()<<db.lastError ().text ();
    }
    else
    {
        qDebug()<<"数据库打开成功";
    }

    createTable(); // 调用函数创建数据库表

    // 设置namelineEdit和codelineEdit为默认回车键接收焦点
    ui->namelineEdit->setFocus();
    ui->namelineEdit->setCursorPosition(0);
    ui->namelineEdit->setPlaceholderText("用户名");
    ui->namelineEdit->setEchoMode(QLineEdit::Normal);

    ui->passwordlineEdit->setEchoMode(QLineEdit::Password);
    ui->passwordlineEdit->setPlaceholderText("密码");

    // 连接回车键按下事件与按钮点击事件
    connect(ui->namelineEdit, &QLineEdit::returnPressed, this, &log_in::on_loadpushButton_clicked);
    connect(ui->passwordlineEdit, &QLineEdit::returnPressed, this, &log_in::on_loadpushButton_clicked);

    passwordVisible = false;

    // 加载数据库中的用户名
    loadUsernamesFromDatabase();
}

log_in::~log_in()
{
    delete ui;

}

void log_in::createTable()
{
    QSqlQuery query;
    query.prepare("CREATE TABLE IF NOT EXISTS users (username TEXT PRIMARY KEY, password TEXT)");

    if (query.exec()) {
        qDebug() << "数据库表创建成功";
    } else {
        qDebug() << "数据库表创建失败: " << query.lastError().text();
    }
}

void log_in::on_newpushButton_clicked() {
    // 获取要注册的用户名和密码
    QString name = QInputDialog::getText(this, "注册", "请输入要注册的用户名:");
    QString code = QInputDialog::getText(this, "注册", "请输入要注册的密码:");

    // 检查用户名和密码是否为空
    if (name.isEmpty() || code.isEmpty()) {
        QMessageBox::information(this, "注册失败", "请输入要注册的用户名和密码");
        return;
    }

    // 检查用户名是否已存在
    QSqlQuery registerQuery;
    registerQuery.prepare("SELECT * FROM users WHERE username = ?");
    registerQuery.addBindValue(name);
    if (registerQuery.exec() && registerQuery.next()) {
        QMessageBox::information(this, "注册失败", "用户名已存在，请重新输入");
        return;
    }

    // 注册新用户
    registerQuery.prepare("INSERT INTO users (username, password) VALUES (?, ?)");
    registerQuery.addBindValue(name);
    registerQuery.addBindValue(code);
    if (registerQuery.exec()) {
        QMessageBox::information(this, "注册成功", "注册成功！");
        loadUsernamesFromDatabase(); // 刷新用户下拉框中的用户名列表
    } else {
        QMessageBox::critical(this, "注册失败", QString("注册失败！\n%1").arg(registerQuery.lastError().text()));
    }
}

void log_in::on_showcodepushButton_clicked()
{
    if (passwordVisible) {
        // 隐藏密码
        ui->passwordlineEdit->setEchoMode(QLineEdit::Password);
        passwordVisible = false;
    } else {
        // 显示密码
        ui->passwordlineEdit->setEchoMode(QLineEdit::Normal);
        passwordVisible = true;
    }
}


void log_in::on_namecomboBox_activated(int index)
{
    Q_UNUSED(index);
    QString selectedUsername = ui->namecomboBox->currentText();
    ui->namelineEdit->setText(selectedUsername);
}


void log_in::on_loadpushButton_clicked()
{
    try {
        QString name = ui->namelineEdit->text();
        QString code = ui->passwordlineEdit->text();

        // 执行查询操作，根据输入的用户名查询数据库中是否存在相应的记录
        QSqlQuery query;
        query.prepare("SELECT * FROM users WHERE username = ?");
        query.addBindValue(name);
        if (query.exec() && query.next()) {
            QString storedPassword = query.value("password").toString();

            // 对比输入的密码与数据库中存储的密码
            if (code == storedPassword) {
                // 登录成功，创建工资管理系统主窗口
                LunarCalendarWidget *l = new LunarCalendarWidget();
                l->show();

                // 关闭当前登录窗口
                this->close();

                // 清空输入框
                ui->namelineEdit->clear();
                ui->passwordlineEdit->clear();

            } else {
                QMessageBox::information(this, "登录失败", "密码错误");
            }
        } else {
            QMessageBox::information(this, "登录失败", "用户名不存在");
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "错误", QString("发生异常: ") + e.what());
    } catch (...) {
        QMessageBox::critical(this, "错误", "未知异常发生");
    }
}

void log_in::loadUsernamesFromDatabase()
{
    ui->namecomboBox->clear(); // 清空用户下拉框中的内容

    // 查询数据库中的用户名
    QSqlQuery query("SELECT username FROM users");
    if (query.exec()) {
        while (query.next()) {
            QString username = query.value("username").toString();
            ui->namecomboBox->addItem(username);
        }
    } else {
        qDebug() << "数据库查询错误: " << query.lastError().text();
    }
}

