#include "login.h"
#include "ui_login.h"
#include"lunarcalendarwidget.h"

#include<QInputDialog>

Login::Login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Login)
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

    // 设置namelineEdit和codelineEdit为默认回车键接收焦点
    ui->namelineEdit->setFocus();
    ui->namelineEdit->setCursorPosition(0);
    ui->namelineEdit->setPlaceholderText("用户名");
    ui->namelineEdit->setEchoMode(QLineEdit::Normal);

    ui->passwordlineEdit->setEchoMode(QLineEdit::Password);
    ui->passwordlineEdit->setPlaceholderText("密码");

    // 连接回车键按下事件与按钮点击事件
    connect(ui->namelineEdit, &QLineEdit::returnPressed, this, &Login::on_loadpushButton_clicked);
    connect(ui->passwordlineEdit, &QLineEdit::returnPressed, this, &Login::on_loadpushButton_clicked);

    passwordVisible = false;

    sqlite_Init();

    // 加载数据库中的用户名到下拉框
    loadUsernamesFromDatabase();
}

Login::~Login()
{
    login_db.close();
    delete ui;
}

void Login::sqlite_Init() {
    login_db = QSqlDatabase::addDatabase("QSQLITE", "login_db");
    login_db.setDatabaseName("users.db");
    if(!login_db.open()) {
        qDebug() << "打开错误";
    }
    QString createsql = QString("CREATE TABLE IF NOT EXISTS users ("
                                "username TEXT PRIMARY KEY,"
                                "password TEXT NOT NULL)");
    QSqlQuery query(login_db);
    if(!query.exec(createsql)) {
        qDebug() << "表创建错误";
    }
    else {
        qDebug() << "表创建成功";
    }
}


void Login::on_namecomboBox_activated(int index)
{
    Q_UNUSED(index);
    QString selectedUsername = ui->namecomboBox->currentText();
    ui->namelineEdit->setText(selectedUsername);
}


void Login::on_showcodepushButton_clicked()
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


void Login::on_loadpushButton_clicked()
{
    QString username = ui->namelineEdit->text();
    QString password = ui->passwordlineEdit->text();
    QString sql = QString("select * from users where username='%1' and password='%2'")
                      .arg(username).arg(password);
    QSqlQuery query(login_db);
    if(query.exec(sql) &&query.next()) {
        qDebug() << "Login success";
        QMessageBox::information(this,"登录认证","登录成功");
        LunarCalendarWidget *l = new LunarCalendarWidget();
        l->show();
        this->close();
    }
    else {
        qDebug() << "Login error";
        QMessageBox::information(this,"登录认证","登录失败,账户或者密码错误");
    }
}


void Login::on_newpushButton_clicked()
{
    // 使用输入对话框获取用户名和密码
    QString username = QInputDialog::getText(this, "注册", "请输入用户名:");
    QString password = QInputDialog::getText(this, "注册", "请输入密码:", QLineEdit::Password);

    // 检查用户名和密码是否为空
    if(username.isEmpty() || password.isEmpty()) {
        QMessageBox::information(this,"注册提示","用户名和密码不能为空");
        return;
    }

    // 检查用户名是否已存在
    QSqlQuery query(login_db);
    query.prepare("SELECT * FROM users WHERE username = ?");
    query.addBindValue(username);
    if(query.exec() && query.next()) {
        QMessageBox::information(this,"注册提示","用户名已存在");
        return;
    }

    // 将新用户插入到数据库中
    QString insertSql = "INSERT INTO users (username, password) VALUES (?, ?)";
    query.prepare(insertSql);
    query.addBindValue(username);
    query.addBindValue(password);
    if(!query.exec()) {
        qDebug() << "注册失败";
        QMessageBox::critical(this,"注册失败","注册失败，请稍后重试");
        return;
    }

    loadUsernamesFromDatabase();

    qDebug() << "注册成功";
    QMessageBox::information(this,"注册成功","注册成功，请登录");
}

void Login::loadUsernamesFromDatabase()
{
    ui->namecomboBox->clear(); // 清空用户下拉框中的内容

    // 查询数据库中的用户名
    QString sql = "SELECT username FROM users";
    QSqlQuery query(login_db);
    if (query.exec(sql)) {
        while (query.next()) {
            QString username = query.value("username").toString();
            ui->namecomboBox->addItem(username);
        }
    } else {
        qDebug() << "数据库查询错误: " << query.lastError().text();
    }
}

