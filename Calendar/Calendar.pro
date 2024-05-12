QT       += core gui
QT       +=sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    arrangedwnd.cpp \
    form.cpp \
    login.cpp \
    lunarcalendarinfo.cpp \
    lunarcalendaritem.cpp \
    lunarcalendarwidget.cpp \
    main.cpp \
    notifymanager.cpp \
    notifywnd.cpp \
    sqlitestorage.cpp \
    to_do_list.cpp

HEADERS += \
    arrangedwnd.h \
    form.h \
    login.h \
    lunarcalendarinfo.h \
    lunarcalendaritem.h \
    lunarcalendarwidget.h \
    notifymanager.h \
    notifywnd.h \
    sqlitestorage.h \
    to_do_list.h \
    todoevent.h

FORMS += \
    form.ui \
    login.ui \
    to_do_list.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
