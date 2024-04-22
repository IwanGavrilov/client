#pragma once

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <fstream>

//namespace asio = boost::asio;
using namespace boost::placeholders;

//using tcp = asio::ip::tcp;
//using err_code = boost::system::error_code;

class TCPClient : public boost::enable_shared_from_this<TCPClient>, boost::noncopyable {
    typedef TCPClient self_type;       
private:
    static const int MAX_LEN_MSG = 1024;           // максимальная длинна чтения очереди

    std::string username_;
    std::string password_;
    std::string command_;                           // команда на сервер
    std::string result_command_;                    // возвращаемый результат
    boost::asio::ip::tcp::socket socket_;
    char read_buffer_[MAX_LEN_MSG];
    char write_buffer_[MAX_LEN_MSG];
    bool started_ {false};
    boost::asio::deadline_timer timer_;
    
    boost::asio::streambuf m_request;
    std::string s_download_file_path{""};           // путь загружаемого файла
public:
    //typedef boost::shared_ptr<TCPClient> ptr_client;
    
    TCPClient(const std::string & username, const std::string & password); 
    TCPClient(boost::asio::io_service & service,const std::string & username, const std::string & password); 
    //        : io_context(context), socket(io_context), b_started(true), username(username), timer(io_context) {};

    void start(boost::asio::ip::tcp::endpoint ep);

    static boost::shared_ptr<TCPClient> start(
                            boost::asio::ip::tcp::endpoint endpoint, 
                            const std::string & username,
                            const std::string & password,
                            const std::string & cmd
                );

    static boost::shared_ptr<TCPClient> start(
                            boost::asio::io_service & service,
                            boost::asio::ip::tcp::endpoint endpoint, 
                            const std::string & username,
                            const std::string & password,
                            const std::string & cmd
                );
    void stop();

    bool get_started();
    
    void set_file_path(std::string s_path);

    std::string get_result_command();
    
private:
    void on_login();
    // отправить команду на сервер
    void do_commands();
    void on_connect(const boost::system::error_code & err);
    void on_commands(const std::string & msg);
    /*
    void on_ping(const std::string & msg);
    void on_clients(const std::string & msg);
    void do_ping();
    void postpone_ping();
    void do_ask_clients();
    */
    void on_read(const boost::system::error_code & err, size_t bytes);
    void on_write(const boost::system::error_code & err, size_t bytes);
    void do_read();
    void do_write(const std::string & msg);
    size_t read_complete(const boost::system::error_code & err, size_t bytes);

    std::string get_command();
    void set_command(const std::string command);

    // открываем файл и пишем с поток
    std::vector<char> open_file();
    // отправляем файл
    void do_file();
};

inline boost::asio::io_service service;
