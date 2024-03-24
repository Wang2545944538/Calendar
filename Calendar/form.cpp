#include "form.h"
#include "ui_form.h"
#include"lunarcalendarwidget.h"

Form::Form(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Form)
{
    ui->setupUi(this);

    // 设置placeholder文本
    ui->name->setPlaceholderText("事件名称");
    ui->location->setPlaceholderText("地点");
    ui->event->setPlaceholderText("事件描述");

    // 获取当前时间
    QDateTime currentDateTime = QDateTime::currentDateTime();
    // 将当前时间设置为dateEdit的日期
    ui->SdateEdit->setDateTime(currentDateTime);
    ui->EdateEdit->setDateTime(currentDateTime);

    ui->startTimeEdit->setEnabled(false);
    ui->endTimeEdit->setEnabled(false);

    // 连接checkBox的stateChanged信号到槽函数
    connect(ui->checkBox, SIGNAL(stateChanged(int)), this, SLOT(onCheckBoxStateChanged(int)));

}

Form::~Form()
{
    delete ui;
}

// 槽函数：当checkBox的状态更改时调用
void Form::on_checkBox_stateChanged(int state)
{
    // 根据checkBox的状态启用或禁用相关的timeEdit部件
    if (state == Qt::Checked) {
        // 如果checkBox被勾选，则禁用timeEdit部件
        ui->startTimeEdit->setEnabled(false);
        ui->endTimeEdit->setEnabled(false);
    } else {
        // 如果checkBox未被勾选，则启用timeEdit部件
        ui->startTimeEdit->setEnabled(true);
        ui->endTimeEdit->setEnabled(true);
    }
}

void Form::on_save_clicked()
{
    // 从各个 UI 控件中获取数据
    QString name = ui->name->text(); // 从 QLineEdit 中读取事件名称
    QString location = ui->location->text(); // 从 QLineEdit 中读取地点
    QString details = ui->event->toPlainText(); // 从 QTextEdit 中读取事件详细信息

    // 如果名称为空，显示消息框并返回
    if (name.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("事件名称不能为空"), QMessageBox::Ok);
        return;
    }

    // 获取开始时间，将 QTimeEdit 和 QDate 合并
    QDateTime startTime(ui->SdateEdit->date(), ui->startTimeEdit->time());

    // 获取结束时间，将 QTimeEdit 和 QDate 合并
    QDateTime endTime(ui->EdateEdit->date(), ui->endTimeEdit->time());


    // 获取comboBox当前选中的文本
    QString priority = ui->comboBox->currentText();

    // 添加判断逻辑，确保结束时间大于等于开始时间
    if (endTime < startTime) {
        QMessageBox::critical(this, tr("Error"), tr("结束时间不能小于开始时间"), QMessageBox::Ok);
        return; // 结束函数，不再继续执行保存操作
    }

    // 创建一个 TodoEvent 对象
    TodoEvent event(name, startTime, endTime, location, details ,priority);

    // 将事件存储到数据库中
    SQLiteStorage storage;
    int id = storage.addEvent(event);
    if (id != -1) {
        qDebug() << "Event added to database with ID:" << id;
    } else {
        qDebug() << "Error adding event to database";
    }


    LunarCalendarWidget lunarCalendarWidget;
    lunarCalendarWidget.updateCalendarEvents();

    this->close();
}
