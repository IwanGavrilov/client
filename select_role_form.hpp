#ifndef SELECT_ROLE_FORM_HPP
#define SELECT_ROLE_FORM_HPP

#include <QDialog>

namespace Ui {
class select_role_form;
}

class select_role_form : public QDialog
{
    Q_OBJECT

public:
    explicit select_role_form(QWidget *parent = nullptr);
    ~select_role_form();

    std::string get_role();

private slots:
    void on_listWidget_clicked(const QModelIndex &index);

private:
    Ui::select_role_form *ui;
    QString s_role {""};
};

#endif // SELECT_ROLE_FORM_HPP
