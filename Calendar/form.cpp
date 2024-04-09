#include "form.h"
#include "ui_form.h"
#include"lunarcalendarwidget.h"

#include<QCloseEvent>
#include<QTimer>

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

    // 设置startTimeEdit和endTimeEdit的默认时间为8点和18点
    QTime defaultStartTime(8, 0); // 默认开始时间为8点
    QTime defaultEndTime(18, 0); // 默认结束时间为18点
    ui->startTimeEdit->setTime(defaultStartTime);
    ui->endTimeEdit->setTime(defaultEndTime);

    ui->startTimeEdit->setEnabled(false);
    ui->endTimeEdit->setEnabled(false);

    // 连接checkBox的stateChanged信号到槽函数
    connect(ui->checkBox, SIGNAL(stateChanged(int)), this, SLOT(on_checkBox_stateChanged(int)));

    setFixedSize(690,450);
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
    // 设置标志位为真，表示由保存按钮触发了关闭操作
    closedBySaveButton = true;

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
    int id = storage.addEvent(event);
    if (id != -1) {
        qDebug() << "Event added to database";
    } else {
        qDebug() << "Error adding event to database";
    }


    LunarCalendarWidget lunarCalendarWidget;
    lunarCalendarWidget.updateCalendarEvents();

    this->close();
}

void Form::closeEvent(QCloseEvent *event) {
    // 从各个 UI 控件中获取数据
    QString name = ui->name->text(); // 从 QLineEdit 中读取事件名称

    // 如果名称不为空，且用户未点击保存按钮，则弹出保存提示框
    if (!name.isEmpty()&& !closedBySaveButton) {
        QMessageBox::StandardButton resBtn = QMessageBox::question(this, tr("保存事件"),
                                                                   tr("是否保存当前事件？"),
                                                                   QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                   QMessageBox::Yes);
        if (resBtn == QMessageBox::Yes) {
            // 如果用户选择保存，则执行保存操作
            on_save_clicked();
        } else if (resBtn == QMessageBox::No) {
            // 如果用户选择不保存，则关闭窗口
            event->accept();
        } else {
            // 如果用户取消关闭操作，则忽略关闭事件
            event->ignore();
        }
    } else {
        // 如果名称为空，直接关闭窗口
        event->accept();
    }
}

void Form::populateEventDetails(const TodoEvent &event)
{
    // 将事件的各个属性填充到表单中的对应控件中
    ui->name->setText(event.name);
    ui->location->setText(event.location);
    ui->event->setText(event.details);

    // 设置开始时间和结束时间
    ui->SdateEdit->setDateTime(event.startTime);
    ui->EdateEdit->setDateTime(event.endTime);

    // 设置优先级
    int index = ui->comboBox->findText(event.priority);
    if (index != -1) {
        ui->comboBox->setCurrentIndex(index);
    }

    // 检查是否需要启用时间编辑控件
    if (event.startTime.time() == QTime(0, 0) && event.endTime.time() == QTime(0, 0)) {
        // 如果开始时间和结束时间的时间部分都是0:0，则说明这个事件是全天事件，勾选全天事件的复选框
        ui->checkBox->setChecked(true);
        ui->startTimeEdit->setEnabled(false);
        ui->endTimeEdit->setEnabled(false);
    } else {
        // 否则，根据事件的具体时间情况来设置时间编辑控件的值和是否启用
        ui->checkBox->setChecked(false);
        ui->startTimeEdit->setEnabled(true);
        ui->endTimeEdit->setEnabled(true);
        ui->startTimeEdit->setTime(event.startTime.time());
        ui->endTimeEdit->setTime(event.endTime.time());
    }
}

void Form::setSelectedDate(const QDate &date)
{
    // 将传递进来的日期设置为起止日期的默认值
    ui->SdateEdit->setDate(date);
    ui->EdateEdit->setDate(date);

    // 确保时间编辑控件处于禁用状态（如果需要的话）
    ui->startTimeEdit->setEnabled(false);
    ui->endTimeEdit->setEnabled(false);
}

