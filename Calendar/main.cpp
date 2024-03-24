#include"lunarcalendarwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LunarCalendarWidget l;
    l.show();
    return a.exec();
}
