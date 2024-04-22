#include "client_async.hpp"

using namespace boost::asio; 
using namespace boost::posix_time;

TCPClient::TCPClient(const std::string & username, const std::string & password) 
        : socket_(service), started_(true), username_(username), password_(password), timer_(service) {
            
        }

TCPClient::TCPClient(boost::asio::io_service & service, const std::string & username, const std::string & password)
        : socket_(service), started_(true), username_(username), password_(password), timer_(service) {

}

void TCPClient::on_connect(const boost::system::error_code & err) {
    if (!err) {
        do_write("connect " + username_ + "#" + password_ + "\n");
    }
    else {
        stop();
    }
}

void TCPClient::do_commands() {
    std::cout << "Command : " << command_ << std::endl;
    do_write(command_ + "\n");
}

void TCPClient::on_read(const boost::system::error_code & err, size_t bytes) {
    if (err) stop();

    if (!get_started()) return;
    
    std::string msg(read_buffer_, bytes);

    if (msg.find("login_ok\n") == 0) {
        std::cout << "login ok" << std::endl;
        do_commands();
        return;
    }
    if (msg.find("login_no\n") == 0) {
        // std::cout << "Error login" << std::endl;
        result_command_ = "Error authentication";
        return;
    }
    if (msg.find("catalogs_ok ") == 0) {
        //std::cout << "Command OK" << std::endl;
        on_commands(msg);
        return;
    }
    if (msg.find("catalogs_no\n") == 0) {
        //std::cout << "Error command" << std::endl;
        result_command_ = "Error command";
        return;
    }
    if (msg.find("add_role_ok\n") == 0) {
        result_command_ = "Add role";
        return;
    }
    if (msg.find("add_role_no\n") == 0) {
        result_command_ = "Error add role";
        return;
    }
    if (msg.find("del_role_ok\n") == 0) {
        result_command_ = "Delete role";
        return;
    }
    if (msg.find("del_role_no\n") == 0) {
        result_command_ = "Error delete role";
        return;
    }
    if (msg.find("add_catalog_ok\n") == 0) {
        result_command_ = "add catalog";
        return;
    }
    if (msg.find("add_catalog_no\n") == 0) {
        result_command_ = "Error add catalog";
        return;
    }
    if (msg.find("add_file_expectation_on\n") == 0) {
        do_file();
    }
    if (msg.find("file_save_ok\n") == 0) {
        result_command_ = "Save file";
        return;
    }
    if (msg.find("file_save_no\n") == 0) {
        result_command_ = "Error save file";
        return;
    }
}

void TCPClient::on_login() {
    std::cout << username_ << " logged in" << std::endl;
    // соединился. запрашиваю списко каталогов
    //do_write("catalogs " + username_);
    //do_ask_clients();
}

void TCPClient::on_commands(const std::string & msg) {
    std::istringstream in(msg);
    std::string answer;
    in >> answer >> answer;
    result_command_ = answer;
    std::cout << answer << std::endl;
}

/*
void TCPClient::on_ping(const std::string & msg) {
    std::istringstream in(msg);
    std::string answer;
    in >> answer >> answer;
    if (answer == "client_list_changed") {
        do_ask_clients();
    }
    else {
        postpone_ping();
    }
}

void TCPClient::on_clients(const std::string & msg) {
    std::string clients = msg.substr(8);
    std::cout << username_ << ", new client list:" << clients ;
    postpone_ping();
}

void TCPClient::do_ping() {
    do_write("add gfshgfhjkgsahgfsak\n");
}

void TCPClient::postpone_ping() {
    int millis = rand() % 7000;
    std::cout << username_ << " postponing ping " << millis 
                  << " millis" << std::endl;
    
    timer_.expires_from_now(boost::posix_time::millisec(millis));
    
    timer_.async_wait(
            boost::bind(&TCPClient::do_ping, shared_from_this())
    );
}

void TCPClient::do_ask_clients() {
    do_write("ask_clients\n");
}
*/
void TCPClient::on_write(const boost::system::error_code & err, size_t bytes) {
    do_read();
}

void TCPClient::do_read() {
    async_read(
            socket_,
            boost::asio::buffer(read_buffer_), 
            boost::bind(&TCPClient::read_complete, shared_from_this(), _1, _2),
            boost::bind(&TCPClient::on_read, shared_from_this(), _1, _2)
    );
}

void TCPClient::do_write(const std::string & msg) {
    if (!get_started()) return;
    
    std::copy(msg.begin(), msg.end(), write_buffer_);
    
    socket_.async_write_some(
            boost::asio::buffer(write_buffer_, msg.size()), 
            boost::bind(&TCPClient::on_write, shared_from_this(), _1, _2)
    );
}

size_t TCPClient::read_complete(const boost::system::error_code & err, size_t bytes) {
    if ( err) return 0;
    bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;
    // we read one-by-one until we get to enter, no buffering
    return found ? 0 : 1;
}

bool TCPClient::get_started() {
    return started_;
}

boost::shared_ptr<TCPClient> TCPClient::start(boost::asio::ip::tcp::endpoint endpoint, const std::string & username, const std::string & password, const std::string & cmd) {
    boost::shared_ptr<TCPClient> new_(new TCPClient(username, password));
    new_->start(endpoint);
    new_->set_command(cmd);
    return new_;
}

boost::shared_ptr<TCPClient> TCPClient::start(
                            boost::asio::io_service & service,
                            boost::asio::ip::tcp::endpoint endpoint, 
                            const std::string & username,
                            const std::string & password,
                            const std::string & cmd
                ) {
    boost::shared_ptr<TCPClient> new_(new TCPClient(service, username, password));
    new_->start(endpoint);
    // если команда add_file, то в первой части имя команды и каталог куда класть файл
    // вторая часть (после "#") источник (полный путь к файлу). Он не нужен в команде
    // третья часть имя файлы. Оно нужно для сохранения на сервере
    if (cmd.find("add_file ") == 0) {
        std::vector<std::string> strs;
        boost::split(strs, cmd, boost::is_any_of("#"));
        std::string s_cmd = strs[0] + "#" + strs[2];
        new_->set_file_path(strs[1]);
        new_->set_command(s_cmd);

    } else {
        new_->set_command(cmd);
    }
    
    return new_;
};

void TCPClient::stop() {
    if (!get_started()) return;
    std::cout << "stopping " << username_ << std::endl;
    started_ = false;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    socket_.close();
}

void TCPClient::start(boost::asio::ip::tcp::endpoint ep) {
    socket_.async_connect(ep, boost::bind(&TCPClient::on_connect, shared_from_this(), _1));
}

std::string TCPClient::get_command() {
    return command_;
}

void TCPClient::set_command(const std::string command) {
    command_ = command;
}

std::vector<char> TCPClient::open_file() {
    std::ifstream m_sourceFile;                     // поток с файлом
    m_sourceFile.open(s_download_file_path, std::ios_base::binary | std::ios_base::ate);
    if (m_sourceFile.fail())
        throw std::fstream::failure("Failed while opening file " + s_download_file_path);
    
    m_sourceFile.seekg(0, m_sourceFile.end);
    auto fileSize = m_sourceFile.tellg();
    std::vector<char> returt_value(fileSize);
    m_sourceFile.seekg(0, m_sourceFile.beg);
    if (fileSize) {
        m_sourceFile.read(&returt_value[0], fileSize);
    }
    m_sourceFile.close();
    return move(returt_value);
}

void TCPClient::do_file() {
    auto bytes = move(open_file());
    socket_.async_write_some(
        boost::asio::buffer(bytes),
        boost::bind(&TCPClient::on_write, shared_from_this(), _1, _2)
    );
}
void TCPClient::set_file_path(std::string s_path) {
    s_download_file_path = s_path;
    std::cout << "File path : " << s_download_file_path << std::endl;
}

std::string TCPClient::get_result_command() {
    return result_command_;
}
