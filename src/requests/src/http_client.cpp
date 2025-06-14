#include "http_client.hpp"

#include <iostream>
#include <boost/asio/ssl/error.hpp> 
#include <boost/asio/ssl/stream.hpp>

std::string HttpClient::SendRequest(
    const std::string& host,
    const std::string& port,
    const std::string& target,
    http::verb method,
    const std::string& body,
    const std::vector<std::pair<std::string, std::string>>& headers)
{
    std::cout << "DEBUG: Entering SendRequest" << std::endl;
    net::io_context ioc;

    ssl::context ctx(ssl::context::method::tlsv12_client);

    ip::tcp::resolver resolver(ioc); 
    sys::error_code ec;
    std::cout << "DEBUG: Before resolve" << std::endl;
    auto const results = resolver.resolve(host, port, ec);
    std::cout << "DEBUG: After resolve" << std::endl;
    if (ec) {
        throw std::runtime_error("DNS error: " + ec.message());
    }

    beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);

    std::cout << "DEBUG: Before stream connect" << std::endl;
    beast::get_lowest_layer(stream).connect(results, ec);
    std::cout << "DEBUG: After stream connect" << std::endl;
    if (ec) {
        throw std::runtime_error("TCP connection error: " + ec.message());
    }
    if constexpr (std::is_same_v<beast::lowest_layer_type<beast::ssl_stream<beast::tcp_stream>>, beast::tcp_stream>) { // Пример проверки SSL
        std::cout << "DEBUG: Before SSL handshake" << std::endl;
        stream.handshake(ssl::stream_base::client, ec);
        std::cout << "DEBUG: After SSL handshake" << std::endl;
    }

    if (ec) {
        throw std::runtime_error("SSL Handshake Error: " + ec.message());
    }

    http::request<http::string_body> req{method, target, 11};
    req.set(http::field::host, host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    req.set(http::field::connection, "close");

    for (const auto& h : headers) { 
        req.set(h.first, h.second);
    }

    if (!body.empty()) { 
        req.body() = body; 
        req.prepare_payload(); 
    }

    std::cout << "DEBUG: Before http write" << std::endl; 
    http::write(stream, req, ec);
    std::cout << "DEBUG: After http write" << std::endl;

    if (ec) { 
        if(ec == net::error::eof) 
            throw std::runtime_error("Sending error: The server closed the connection prematurely"); 
        else if (ec == ssl::error::stream_truncated)
            throw std::runtime_error("Sending error (SSL): The stream is truncated during recording");
        else
            throw std::runtime_error("Sending error: " + ec.message());
    }

    beast::flat_buffer buffer; 
    http::response<http::string_body> res;

    std::cout << "DEBUG: Before http read" << std::endl; 
    http::read(stream, buffer, res, ec);
    std::cout << "DEBUG: After http read" << std::endl;

    if (ec && ec != http::error::end_of_stream) {
        if (ec == ssl::error::stream_truncated)
            throw std::runtime_error("Response Read error (SSL): The stream is truncated during reading");
        else
            throw std::runtime_error("Error reading the response: " + ec.message());
    }

    if (res.result() != http::status::ok) { 
        std::string error_msg = "The server returned the error status: " + std::to_string(res.result_int());
        if (!res.body().empty()) {
            error_msg += ". Response body: " + res.body();
        }
        throw std::runtime_error(error_msg);
    }

    std::cout << "DEBUG: Before stream shutdown" << std::endl;
    stream.shutdown(ec);

    if (ec && ec != beast::errc::not_connected && ec != ssl::error::stream_truncated) {
    }
    std::cout << "DEBUG: Exiting SendRequest" << std::endl;
    return res.body(); 
}


/*
HttpClient::HttpClient() : ioc_() {}

HttpClient::~HttpClient() {}

std::string HttpClient::get(const std::string& host, const std::string& port, const std::string& target) {
    try {
        // 1. Создаем resolver для поиска IP адреса по имени хоста
        tcp::resolver resolver(ioc_);
        
        // 2. Разрешаем имя хоста и порт в список конечных точек
        auto const results = resolver.resolve(host, port);
        
        // 3. Создаем сокет и подключаемся к серверу
        tcp::socket socket(ioc_);
        net::connect(socket, results.begin(), results.end());
        
        // 4. Создаем HTTP GET запрос
        http::request<http::string_body> req{http::verb::get, target, 11};
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        
        // 5. Отправляем запрос
        http::write(socket, req);
        
        // 6. Получаем ответ
        beast::flat_buffer buffer;
        http::response<http::dynamic_body> res;
        http::read(socket, buffer, res);
        
        // 7. Закрываем соединение
        beast::error_code ec;
        socket.shutdown(tcp::socket::shutdown_both, ec);
        
        // 8. Преобразуем ответ в строку и возвращаем
        return boost::beast::buffers_to_string(res.body().data());
    }
    catch(std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return "";
    }
}

std::string HttpClient::post(const std::string& host, const std::string& port, 
                           const std::string& target, const std::string& body) {
    try {
        // 1. Создаем resolver для поиска IP адреса по имени хоста
        tcp::resolver resolver(ioc_);
        
        // 2. Разрешаем имя хоста и порт в список конечных точек
        auto const results = resolver.resolve(host, port);
        
        // 3. Создаем сокет и подключаемся к серверу
        tcp::socket socket(ioc_);
        net::connect(socket, results.begin(), results.end());
        
        // 4. Создаем HTTP POST запрос
        http::request<http::string_body> req{http::verb::post, target, 11};
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set(http::field::content_type, "application/json");
        req.body() = body;
        req.prepare_payload();
        
        // 5. Отправляем запрос
        http::write(socket, req);
        
        // 6. Получаем ответ
        beast::flat_buffer buffer;
        http::response<http::dynamic_body> res;
        http::read(socket, buffer, res);
        
        // 7. Закрываем соединение
        beast::error_code ec;
        socket.shutdown(tcp::socket::shutdown_both, ec);
        
        // 8. Преобразуем ответ в строку и возвращаем
        return boost::beast::buffers_to_string(res.body().data());
    }
    catch(std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return "";
    }
}
*/