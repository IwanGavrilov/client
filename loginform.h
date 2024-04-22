#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QDialog>

#include <string>

namespace Ui {
class LoginForm;
}

// class QLineEdit;

class LoginForm : public QDialog
{
    Q_OBJECT

public:
    explicit LoginForm(QWidget *parent = nullptr);
    ~LoginForm();

    std::string get_login();
    std::string get_password();

private:
    Ui::LoginForm *ui;

//    QLineEdit * ptr_login;
//    QLineEdit * ptr_password;

    std::string s_login {""};
    std::string s_password {""};
};

#endif // LOGINFORM_H
