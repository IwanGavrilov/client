#include "loginform.h"
#include "ui_loginform.h"

LoginForm::LoginForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginForm)
{
    ui->setupUi(this);
}

LoginForm::~LoginForm()
{
    delete ui;
}

std::string LoginForm::get_login() {
    return ui->edt_login->text().toStdString();
}

std::string LoginForm::get_password() {
    return ui->edt_password->text().toStdString();
}
