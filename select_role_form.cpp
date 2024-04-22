#include "select_role_form.hpp"
#include "ui_select_role_form.h"

select_role_form::select_role_form(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::select_role_form)
{
    ui->setupUi(this);
    ui->listWidget->insertItem(0, "Разработчик");
    ui->listWidget->insertItem(1, "Просмотр");
}

select_role_form::~select_role_form()
{
    delete ui;
}

std::string select_role_form::get_role() {
    return this->s_role.toStdString();
}

void select_role_form::on_listWidget_clicked(const QModelIndex &index)
{
    QListWidgetItem * item = ui->listWidget->item(index.row());//takeItem(index.row());
    s_role = item->text();
}

