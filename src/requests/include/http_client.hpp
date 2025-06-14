#ifndef HTTP_CLIENT_HPP
#define HTTP_CLIENT_HPP

#include <string>
#include <vector>
#include <utility>
#include <stdexcept>

#include <boost/beast/core.hpp> 
#include <boost/beast/http.hpp> 
#include <boost/beast/version.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/system/error_code.hpp> 

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ip = net::ip;
namespace sys = boost::system;
namespace ssl = net::ssl;

class HttpClient {
public:
    std::string SendRequest(
        const std::string& host,
        const std::string& port,
        const std::string& target,
        http::verb method,
        const std::string& body = "",
        const std::vector<std::pair<std::string, std::string>>& headers = {} // headers - Дополнительные заголовки запроса. Это список пар "Имя заголовка" - "Значение заголовка". По умолчанию - пустой список.
    );

}; 


/*
// Класс для выполнения HTTP запросов
class HttpClient {
public:
    // Конструктор
    HttpClient();
    
    // Деструктор
    ~HttpClient();
    
    // Выполнение GET запроса
    std::string get(const std::string& host, const std::string& port, const std::string& target);
    
    // Выполнение POST запроса
    std::string post(const std::string& host, const std::string& port, 
                    const std::string& target, const std::string& body);

private:
    net::io_context ioc_; // Объект ввода-вывода, необходимый для всех операций ввода-вывода
};
*/

#endif