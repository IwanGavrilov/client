#include "select_user.hpp"
#include "ui_select_user.h"

select_user::select_user(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::select_user)
{
    ui->setupUi(this);

    ui->listWidget->addItem("iwan");
    ui->listWidget->addItem("jule");
    ui->listWidget->addItem("Mike");
    ui->listWidget->addItem("Vlag");
    ui->listWidget->addItem("Olga");
}

select_user::~select_user()
{
    delete ui;
}

void select_user::on_listWidget_clicked(const QModelIndex &index)
{
    QListWidgetItem * item = ui->listWidget->item(index.row());
    s_user = item->text().toStdString();
}

std::string select_user::get_user() {
    return this->s_user;
}

