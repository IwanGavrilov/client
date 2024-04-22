#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    LoginForm * log = new LoginForm();
    if (log->exec() == QDialog::Accepted) {
        s_login = log->get_login();
        s_password = log->get_password();
        boost::asio::io_service service;
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(s_ip_adress_), port_);
        ptr_client = TCPClient::start(service, ep, s_login, s_password, "catalogs " + s_login);
        service.run();
        std::string result = ptr_client->get_result_command();
        ptr_client->stop();
        // инициализирую данные по каталогам и ролям у данного пользователя
        if (initialize_data(result)) {
            for(Folder fld : work_folder.child_folders) {
                QTreeWidgetItem *topLevelItem=new QTreeWidgetItem(ui->treeWidget);
                // вешаем его на наше дерево в качестве топ узла.
                ui->treeWidget->addTopLevelItem(topLevelItem);
                // укажем текст итема
                topLevelItem->setText(0, QString::fromUtf8(fld.s_name.c_str()));
                QIcon icon_dev;
                icon_dev.addFile("/media/waine-86/WORK/My_Project_C_Delphy/OTUS/Project/Sources/Client/UI/client/folder_edit.ico");
                QIcon icon_view;
                icon_view.addFile("/media/waine-86/WORK/My_Project_C_Delphy/OTUS/Project/Sources/Client/UI/client/folder_information.ico");
                if (fld.s_role == "r_develop") {
                    topLevelItem->setIcon(0, icon_dev);
                }
                else {
                    topLevelItem->setIcon(0, icon_view);
                }
                // создаем новый итем и сразу вешаем его на наш базовый
                // QTreeWidgetItem *item=new QTreeWidgetItem(topLevelItem);
                // укажем текст итема
                // item->setText(0,"Под итем");
            }
            ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(ui->treeWidget, &QTreeWidget::customContextMenuRequested, this, &MainWindow::prepareMenu);
        }
        else {
            QMessageBox::critical(NULL,QObject::tr("Ошибка"),tr("Список каталогов не загружен"));
        }
    }
}

Folder * MainWindow::find_parent_folder(Folder & find_folder, std::string s_path_folder) {
    Folder * result_fld = nullptr;
    std::vector<std::string> strs;
    boost::split(strs, s_path_folder, boost::is_any_of("/"));
    s_path_folder = "";
    if (strs.size() == 0) {
        return result_fld;
    }
    for(Folder & curr_folder : find_folder.child_folders) {
        if (curr_folder.s_name == strs[0]) {
            if (strs.size() == 1) {
                return & curr_folder;
            }
            else {
                for(int idx = 1; idx < strs.size(); idx++) {
                    if (idx == 1) { s_path_folder += strs[idx]; } else {s_path_folder += "/" + strs[idx]; }
                }
                result_fld = find_parent_folder(curr_folder, s_path_folder);
            }
        }
    }
    return result_fld;
}

bool MainWindow::initialize_data(std::string s_result) {
    bool b_flag = true;
    QString qs_result = QString::fromUtf8(s_result.c_str());
    QJsonDocument json_doc = QJsonDocument::fromJson(qs_result.toUtf8());
    QJsonArray arr_catalogs = json_doc.array();
    for(const QJsonValue & value : arr_catalogs) {
        if (value.isObject()) {
            std::string s_name_folder = value["folder"].toString().toStdString();
            std::string s_role_folder = value["role"].toString().toStdString();
            Folder new_folder = Folder(s_name_folder, s_role_folder);
            for (const QJsonValue file_name : value["files"].toArray()) {
                new_folder.add_file(file_name.toString().toStdString());
            }
            this->work_folder.add_child_folder(new_folder);
        }
    }

    return b_flag;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    QString qs_str = item->text(column);
    for(Folder curr_folder : work_folder.child_folders) {
        if (QString::fromUtf8(curr_folder.s_name.c_str()) == qs_str) {
            ui->listWidget->clear();
            QIcon icon_file;
            if (curr_folder.s_role == "r_develop") {
                icon_file.addFile("/media/waine-86/WORK/My_Project_C_Delphy/OTUS/Project/Sources/Client/UI/client/form_green.ico");
            } else {
                icon_file.addFile("/media/waine-86/WORK/My_Project_C_Delphy/OTUS/Project/Sources/Client/UI/client/form_blue.ico");
            }
            for (std::string s_file_name : curr_folder.files) {
                // ui->listWidget->addItem(QString::fromUtf8(s_file_name.c_str()));
                QListWidgetItem * item = new QListWidgetItem(ui->listWidget);
                item->setIcon(icon_file);
                item->setText(QString::fromUtf8(s_file_name.c_str()));
            }
        }
    }
    //QMessageBox::information(this, qs_str, qs_str);
}

void MainWindow::prepareMenu(const QPoint & pos) {
    QTreeWidget * tree = ui->treeWidget;
    nd = tree->itemAt(pos);
    qDebug() << pos << nd->text(0);

    m_pmnu = new QMenu(this);
    m_pmnu->addAction("&Добавить роль");
    m_pmnu->addAction("&Удалить роль");
    m_pmnu->addAction("Загрузить &каталог");
    m_pmnu->addAction("Загрузить &файл");

    connect(m_pmnu, SIGNAL(triggered(QAction*)), SLOT(slotActivated(QAction*)));
    /*QAction *newAct = new QAction(QIcon(":/Resource/warning32.ico"), tr("&New"), this);
    newAct->setStatusTip(tr("new sth"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newDev()));
    QMenu menu(this);
    menu.addAction(newAct);*/
    QPoint pt(pos);
    m_pmnu->exec(tree->mapToGlobal(pos));
}

void MainWindow::slotActivated(QAction* pAction) {
    QString strAction = pAction->text().remove("&");

    std::string s_command {""};

    std::string s_name_node = nd->text(0).toStdString();
    if (strAction == "Добавить роль") {
        std::string s_role {""};
        std::string s_user {""};
        select_role_form * role_form = new select_role_form();
        if (role_form->exec() == QDialog::Accepted) {
            /*if (role_form->get_role() == "Разработчик") { s_role = "r_develop"; }
            if (role_form->get_role() == "Просмотр") { s_role = "r_viewer"; }*/
            s_role = role_form->get_role();
        }

        select_user * user_form = new select_user();
        if (user_form->exec() == QDialog::Accepted) {
            s_user = user_form->get_user();
        }
        s_command = "set_role " + s_user + "#" + s_role + "#" + s_name_node;
    }
    if (strAction == "Удалить роль") {
        std::string s_role {""};
        std::string s_user {""};
        select_role_form * role_form = new select_role_form();
        if (role_form->exec() == QDialog::Accepted) {
            /*if (role_form->get_role() == "Разработчик") { s_role = "r_develop"; }
            if (role_form->get_role() == "Просмотр") { s_role = "r_viewer"; }*/
            s_role = role_form->get_role();
        }

        select_user * user_form = new select_user();
        if (user_form->exec() == QDialog::Accepted) {
            s_user = user_form->get_user();
        }
        s_command = "del_role " + s_user + "#" + s_role + "#" + s_name_node;
    }

    if (strAction == "Загрузить каталог") {

    }

    if (strAction == "Загрузить файл") {
        std::string s_path_file = QFileDialog::getOpenFileName(0, "Открыть", "", "*").toStdString();
        std::vector<std::string> strs;
        boost::split(strs, s_path_file, boost::is_any_of("/"));
        std::string s_file_name = strs[strs.size() - 1];        // последним в пути будет имя файла
        s_command = "add_file " + s_name_node + "#" + s_path_file + "#" + s_file_name;
    }
    // запускаем команду
    if (s_command != "") {
        boost::asio::io_service service;
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(s_ip_adress_), port_);
        ptr_client = TCPClient::start(service, ep, s_login, s_password, s_command);
        service.run();
        std::string result = ptr_client->get_result_command();
        qDebug() << result.c_str();
        ptr_client->stop();
    }
}
