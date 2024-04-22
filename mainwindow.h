#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtGui>

#include <QMessageBox>
#include <QTreeWidgetItem>

#include <loginform.h>
#include <client_async.hpp>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QFileDialog>

#include <folder.hpp>
#include <select_role_form.hpp>
#include <select_user.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void prepareMenu( const QPoint & pos );
    ~MainWindow();

private slots:
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);
    void slotActivated(QAction* pAction);

private:
    Ui::MainWindow *ui;
    QMenu* m_pmnu;
    // необходим для построения дерева
    QTreeWidgetItem * nd;

    // логин и пароль пользователя
    std::string s_login {""};
    std::string s_password {""};

    std::uint16_t port_ {15001};
    std::string s_ip_adress_ {"127.0.0.1"};

    boost::shared_ptr<TCPClient> ptr_client;

    Folder work_folder = Folder("SrvTCP", "r_develop");

    bool initialize_data(std::string s_result);

    // возвращаю родителя по указанному пути
    Folder * find_parent_folder(Folder & find_folder, std::string s_path_folder);
};
#endif // MAINWINDOW_H
