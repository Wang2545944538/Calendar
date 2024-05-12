#include"login.h"
#include "notifymanager.h"
#include "sqlitestorage.h"

#include <QApplication>
#include <QMessageBox>
#include<QDialogButtonBox>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 创建全局 NotifyManager 实例
    NotifyManager *manager = new NotifyManager();

    // 可选修改默认参数
    manager->setMaxCount(10); // 最大显示消息数，默认5
    manager->setDisplayTime(9000); // 显示时间，默认10000（毫秒）
    manager->setAnimateTime(500); // 动画时间，默认300（毫秒）
    manager->setSpacing(5); // 消息框间距，默认10px
    manager->setCornerMargins(20, 20); // 右下角边距，默认10, 10
    manager->setNotifyWndSize(300, 75); // 消息框大小，默认300, 60
    manager->setDefaultIcon(":/resources/image/message.png"); // 消息图标，默认":/resources/image/message.png"
    manager->setShowQueueCount(true); // 是否显示超出最大数未显示的消息数量，默认true
    manager->setStyleSheet("#notify-background {....", "自定义主题名称"); // 添加自定义主题样式表，默认样式主题名为default

    // 点击消息处理
    QObject::connect(manager, &NotifyManager::notifyDetail, [&](const QVariantMap &data){
        QMessageBox msgbox(QMessageBox::Information, QStringLiteral("事件提醒"), data.value("body").toString());
        msgbox.setDetailedText(data.value("details").toString());
        msgbox.findChild<QDialogButtonBox*>()->setMinimumWidth(500);
        msgbox.exec();
    });

    // 创建定时器，每分钟触发一次
    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&](){
        // 获取当前时间
        QDateTime currentTime = QDateTime::currentDateTime();
        // 计算15分钟后的时间
        QDateTime reminderTime = currentTime.addSecs(15 * 60);
        //qDebug()<<currentTime<<reminderTime;
        // 从数据库中获取所有事件
        SQLiteStorage &storage = SQLiteStorage::getInstance();
        QVector<TodoEvent> events = storage.getAllEvents();

        // 遍历所有事件，提醒即将开始的事件
        for(const TodoEvent &event : events) {
            if(event.startTime <= reminderTime && event.startTime >=currentTime) {
                QVariantMap data;
                data["details"] = event.details; // 将事件详情添加到提醒的数据中
                manager->notify("事件提醒", event.name + " 即将开始！", data);
            }
        }
    });

    // 启动定时器，每半分钟触发一次
    timer.start(10000); // 10秒

    // 创建并显示您的主窗口或其他 UI 元素
    Login l;
    l.show();

    return a.exec();
}
