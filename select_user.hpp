#ifndef SELECT_USER_HPP
#define SELECT_USER_HPP

#include <QDialog>

namespace Ui {
class select_user;
}

class select_user : public QDialog
{
    Q_OBJECT

public:
    explicit select_user(QWidget *parent = nullptr);
    ~select_user();

    std::string get_user();
private slots:
    void on_listWidget_clicked(const QModelIndex &index);

private:
    Ui::select_user *ui;

    std::string s_user {""};
};

#endif // SELECT_USER_HPP
