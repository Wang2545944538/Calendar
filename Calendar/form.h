#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include<QMessageBox>
#include "ui_form.h"
#include "todoevent.h"
#include"sqlitestorage.h"

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();

    Ui::Form *ui;

private slots:
    void on_checkBox_stateChanged(int state);
    void on_save_clicked();
};

#endif // FORM_H
