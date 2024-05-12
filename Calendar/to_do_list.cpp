#include "to_do_list.h"
#include "ui_to_do_list.h"


To_do_list::To_do_list(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::To_do_list)
{
    ui->setupUi(this);
    this->setWindowTitle("ToDoList");

    ui->tableWidget->verticalHeader()->setVisible(false); //隐藏垂直表头

    ui->tableWidget->horizontalHeader()->setSectionsMovable(false);       //表头不可移动
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);  //表格内容不可编辑
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); //列选择

    ui->tableWidget->setColumnCount(7);
    // 设置表头
    QStringList headerText;
    headerText << "ID"
               << "名称"
               << "起始时间"
               << "终止时间"
               << "优先级"
               << "地点"
               << "详情";
    ui->tableWidget->setHorizontalHeaderLabels(headerText);

    //实现全选
    connect(ui->seleteall, &QPushButton::clicked, ui->tableWidget, [=](bool check) {
        ui->tableWidget->selectAll();
    });

    // 连接表头点击信号到自定义的槽函数
    connect(ui->tableWidget->horizontalHeader(), &QHeaderView::sectionClicked, this, &To_do_list::sortTable);

    setFixedSize(940, 620);
}

To_do_list::~To_do_list()
{
    delete ui;
}

void To_do_list::showAllEvents()
{

    QVector<QPair<int, TodoEvent>> eventsWithIDs = storage.getAllEventsWithIDs();

    // 清空表格部件
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0); // 清除所有行

    // 在表格部件中显示事件
    for (int i = 0; i < eventsWithIDs.size(); ++i) {
        const QPair<int, TodoEvent> &eventPair = eventsWithIDs.at(i);
        int id = eventPair.first;
        const TodoEvent &event = eventPair.second;

        // 添加新行
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);

        // 在新行中显示事件详细信息
        QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(id)); // 获取事件ID并转换为字符串
        QTableWidgetItem *nameItem = new QTableWidgetItem(event.name);
        QTableWidgetItem *startTimeItem = new QTableWidgetItem(event.startTime.toString());
        QTableWidgetItem *endTimeItem = new QTableWidgetItem(event.endTime.toString());
        QTableWidgetItem *priorityItem = new QTableWidgetItem(event.priority);
        QTableWidgetItem *locationItem = new QTableWidgetItem(event.location);
        QTableWidgetItem *detailsItem = new QTableWidgetItem(event.details);


        ui->tableWidget->setItem(row, 0, idItem);         // ID
        ui->tableWidget->setItem(row, 1, nameItem);       // 名称
        ui->tableWidget->setItem(row, 2, startTimeItem); // 开始时间
        ui->tableWidget->setItem(row, 3, endTimeItem);   // 结束时间
        ui->tableWidget->setItem(row, 4, priorityItem);  // 优先级
        ui->tableWidget->setItem(row, 5, locationItem);  // 地点
        ui->tableWidget->setItem(row, 6, detailsItem);   // 详情

        sortTableByTime(2); // 按时间降序排序

        ui->tableWidget->resizeColumnsToContents(); //自动调整列宽
    }
}

void To_do_list::showEventsOnDate(const QDate &date)
{
    // 获取指定日期的事件
    QVector<QPair<int, TodoEvent>> eventsOnDate;

    // 获取所有事件
    QVector<QPair<int, TodoEvent>> allEvents = storage.getAllEventsWithIDs();

    // 遍历所有事件
    for (const auto &eventPair : allEvents) {
        const TodoEvent &event = eventPair.second;

        // 如果事件的开始日期早于或等于指定日期，并且结束日期晚于或等于指定日期，则添加到显示事件中
        if ((event.startTime.date() <= date && event.endTime.date() >= date) ||
            (event.startTime.date() <= date && event.endTime.date() == date) ||
            (event.startTime.date() == date && event.endTime.date() >= date)) {
            eventsOnDate.append(eventPair);
        }
    }

    // 清空表格内容
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    // 在表格中显示指定日期的事件
    for (const auto &eventPair : eventsOnDate) {
        int id = eventPair.first;
        const TodoEvent &event = eventPair.second;

        // 添加新行
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);

        // 在新行中显示事件详细信息
        QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(id));
        QTableWidgetItem *nameItem = new QTableWidgetItem(event.name);
        QTableWidgetItem *startTimeItem = new QTableWidgetItem(event.startTime.toString());
        QTableWidgetItem *endTimeItem = new QTableWidgetItem(event.endTime.toString());
        QTableWidgetItem *priorityItem = new QTableWidgetItem(event.priority);
        QTableWidgetItem *locationItem = new QTableWidgetItem(event.location);
        QTableWidgetItem *detailsItem = new QTableWidgetItem(event.details);

        ui->tableWidget->setItem(row, 0, idItem);
        ui->tableWidget->setItem(row, 1, nameItem);
        ui->tableWidget->setItem(row, 2, startTimeItem);
        ui->tableWidget->setItem(row, 3, endTimeItem);
        ui->tableWidget->setItem(row, 4, priorityItem);
        ui->tableWidget->setItem(row, 5, locationItem);
        ui->tableWidget->setItem(row, 6, detailsItem);

        sortEventsByPriority(); // 按优先级排序
        ui->tableWidget->resizeColumnsToContents(); // 自动调整列宽
    }
}

void To_do_list::deleteSelectedRows() {
    // 获取选中的行号
    QModelIndexList selectedIndexes = ui->tableWidget->selectionModel()->selectedRows();

    // 如果没有选中的行，则直接返回
    if (selectedIndexes.isEmpty()) {
        return;
    }

    // 弹出确认对话框
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "删除确认", "确定要删除选中的事件吗？",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No) {
        return;
    }

    // 创建一个 QVector 用于存储选中行的事件ID
    QVector<int> selectedEventIDs;

    // 从选中的行中获取事件ID
    foreach (const QModelIndex &index, selectedIndexes) {
        int row = index.row();
        QTableWidgetItem *idItem = ui->tableWidget->item(row, 0);
        int id = idItem->text().toInt();
        selectedEventIDs.append(id);
    }

    // 从数据库中删除选中的事件
    foreach (int id, selectedEventIDs) {
        storage.deleteEvent(id);
    }

    // 从表格中删除选中的行
    std::sort(selectedIndexes.begin(), selectedIndexes.end(), [](const QModelIndex &index1, const QModelIndex &index2) {
        return index1.row() > index2.row(); // 按行号降序排序
    });

    std::reverse(selectedIndexes.begin(), selectedIndexes.end()); // 反转顺序，实现降序排序
    foreach (const QModelIndex &index, selectedIndexes) {
        int row = index.row();
        ui->tableWidget->removeRow(row);
    }

    // 更新日历事件
    LunarCalendarWidget lunarCalendarWidget;
    lunarCalendarWidget.updateCalendarEvents();

    // 关闭当前窗口
    this->close();
}

void To_do_list::on_Deleteitem_clicked()
{
    deleteSelectedRows();
}

void To_do_list::on_changeEvent_clicked()
{
    // 获取选中的行号
    QModelIndexList selectedIndexes = ui->tableWidget->selectionModel()->selectedRows();

    // 如果没有选中的行，则显示提示消息框
    if (selectedIndexes.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先选择要修改的事件。"), QMessageBox::Ok);
        return;
    }

    // 如果选中多个事件，提示只能修改一个事件
    if (selectedIndexes.size() > 1) {
        QMessageBox::warning(this, tr("警告"), tr("只能修改一个事件，请取消多余的选择。"), QMessageBox::Ok);
        return;
    }

    // 获取选中的行号
    QModelIndex selectedIndex = selectedIndexes.at(0);
    int row = selectedIndex.row();

    // 获取事件ID
    QTableWidgetItem *idItem = ui->tableWidget->item(row, 0);
    int id = idItem->text().toInt(); // 假设ID是整数

    // 从数据库中获取该ID对应的事件信息
    TodoEvent event = storage.getEvent(id);

    // 创建一个新的表单用于修改事件信息
    Form *form = new Form();
    // 将事件信息填充到表单中
    form->populateEventDetails(event);
    form->show();

    // 从数据库中删除选中的事件
    storage.deleteEvent(id);

    // 从表格中删除选中的行
    ui->tableWidget->removeRow(row);

    // 关闭当前窗口
    this->close();
}


enum Priority {
    High,
    Middle,
    Low
};

// 根据优先级字符串返回相应的枚举值
Priority priorityFromString(const QString& priorityStr) {
    if (priorityStr == "High") {
        return High;
    } else if (priorityStr == "Middle") {
        return Middle;
    } else {
        return Low;
    }
}

// 比较函数，按照优先级从高到低排序
bool compareByPriority(const TodoEvent &event1, const TodoEvent &event2) {
    return priorityFromString(event1.priority) > priorityFromString(event2.priority);
}

void To_do_list::sortTableByTime(int Column) {
    // 使用 QList 存储行数据
    QList<QList<QVariant>> rowsData;

    // 获取表格中的行数和列数
    int rowCount = ui->tableWidget->rowCount();
    int columnCount = ui->tableWidget->columnCount();

    // 从表格中读取数据
    for (int row = 0; row < rowCount; ++row) {
        QList<QVariant> rowData;
        for (int col = 0; col < columnCount; ++col) {
            QTableWidgetItem* item = ui->tableWidget->item(row, col);
            rowData.append(item ? QVariant(item->text()) : QVariant());
        }
        rowsData.append(rowData);
    }

    // 按日期时间排序
    std::sort(rowsData.begin(), rowsData.end(), [Column](const QList<QVariant> &rowData1, const QList<QVariant> &rowData2) {
        QString str1 = rowData1.at(Column).toString();
        QString str2 = rowData2.at(Column).toString();

        QDateTime dateTime1 = QDateTime::fromString(str1, "ddd MMM d hh:mm:ss yyyy");
        QDateTime dateTime2 = QDateTime::fromString(str2, "ddd MMM d hh:mm:ss yyyy");

        return dateTime1 < dateTime2;
    });


    // 清空表格内容
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    // 将排序后的数据重新填充到表格中
    for (int row = 0; row < rowsData.size(); ++row) {
        const QList<QVariant>& rowData = rowsData.at(row);
        ui->tableWidget->insertRow(row);
        for (int col = 0; col < rowData.size(); ++col) {
            QVariant data = rowData.at(col);
            QString text = data.toString();
            ui->tableWidget->setItem(row, col, new QTableWidgetItem(text));
        }
    }
}



void To_do_list::sortEventsByPriority() {
    // 使用 QList 存储行数据
    QList<QList<QVariant>> rowsData;

    // 获取表格中的行数和列数
    int rowCount = ui->tableWidget->rowCount();
    int columnCount = ui->tableWidget->columnCount();

    // 从表格中读取数据
    for (int row = 0; row < rowCount; ++row) {
        QList<QVariant> rowData;
        for (int col = 0; col < columnCount; ++col) {
            QTableWidgetItem* item = ui->tableWidget->item(row, col);
            rowData.append(item ? QVariant(item->text()) : QVariant());
        }
        rowsData.append(rowData);
    }

    // 按优先级排序
    std::sort(rowsData.begin(), rowsData.end(), [](const QList<QVariant> &rowData1, const QList<QVariant> &rowData2) {
        QString priority1 = rowData1.at(4).toString(); // 优先级在第5列
        QString priority2 = rowData2.at(4).toString();
        return priorityFromString(priority1) < priorityFromString(priority2);
    });

    // 清空表格内容
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    // 将排序后的数据重新填充到表格中
    for (int row = 0; row < rowsData.size(); ++row) {
        const QList<QVariant>& rowData = rowsData.at(row);
        ui->tableWidget->insertRow(row);
        for (int col = 0; col < rowData.size(); ++col) {
            QVariant data = rowData.at(col);
            QString text = data.toString();
            ui->tableWidget->setItem(row, col, new QTableWidgetItem(text));
        }
    }
}


void To_do_list::sortTable(int index)
{
    switch (index) {
    case 2: // "起始时间"列头
        sortTableByTime(2); // 按时间顺序排序
        break;
    case 3: // "终止时间"列头
        sortTableByTime(3); // 按时间顺序排序
        break;
    case 4: // "优先级"列头
        sortEventsByPriority(); // 按优先级降序排序
        break;
    default:
        break;
    }
}


void To_do_list::on_search_button_clicked()
{
    // 获取搜索框中的文本内容
    QString searchText = ui->search_edit->text().trimmed(); // 去除首尾空格

    // 如果搜索框内容为空，则直接返回
    if (searchText.isEmpty()) {
        return;
    }

    clearHighlightedText();

    // 定义匹配到的文本的颜色
    QColor highlightColor(Qt::yellow);

    // 遍历表格中的每一行
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        // 遍历每一列
        for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
            QTableWidgetItem *item = ui->tableWidget->item(row, col);
            if (item) {
                // 获取单元格文本内容进行匹配
                QString cellText = item->text();
                // 查找匹配的部分
                int index = cellText.indexOf(searchText, 0, Qt::CaseInsensitive); // 不区分大小写匹配
                while (index != -1) {
                    // 找到匹配内容，将单元格中匹配到的部分标记为黄色
                    item->setBackground(highlightColor);
                    // 移动搜索的起始位置到下一个位置
                    index = cellText.indexOf(searchText, index + searchText.length(), Qt::CaseInsensitive);
                }
            }
        }
    }
}

void To_do_list::clearHighlightedText()
{
    // 遍历表格中的每一行
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        // 遍历每一列
        for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
            QTableWidgetItem *item = ui->tableWidget->item(row, col);
            if (item) {
                // 恢复单元格文本的默认背景色
                item->setBackground(QColor(Qt::white));
            }
        }
    }
}







