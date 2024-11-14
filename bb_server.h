#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/http.hpp>
#include <boost/bind/bind.hpp>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <iostream>
#include <memory>
#include <map>
#include <string>
#include <functional>

using boost::asio::ip::tcp;
namespace beast = boost::beast;
namespace ssl = boost::asio::ssl;
namespace http = beast::http;

// 핸들러 함수 타입 정의
using handler_function = std::function<void(const boost::beast::http::request<boost::beast::http::string_body>&, 
                                            boost::beast::http::response<boost::beast::http::string_body>&)>;

class Server {
public:
    // Singleton pattern
    static Server* instance();

    // 서버 시작 함수
    void run();

    // HTTP 메소드와 URI에 따른 동작을 처리하는 함수
    void handle_request(beast::ssl_stream<beast::tcp_stream>& stream);

private:
    // private 생성자, 복사 생성자, 할당 연산자
    Server();
    // singleton으로 구현하기 위해 복사 생성자/할당 연산자를 delete
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    // HTTP 메소드 처리 함수들
    void handle_ctl_play(const http::request<http::string_body>& req, http::response<http::string_body>& res);
    void handle_ctl_stop(const http::request<http::string_body>& req, http::response<http::string_body>& res);
    void handle_ctl_rewind(const http::request<http::string_body>& req, http::response<http::string_body>& res);

    void handle_get_bestshot(const http::request<http::string_body>& req, http::response<http::string_body>& res);
    void handle_get_value(const http::request<http::string_body>& req, http::response<http::string_body>& res);
    void handle_get_ocr(const http::request<http::string_body>& req, http::response<http::string_body>& res);
    void handle_get_dir(const http::request<http::string_body>& req, http::response<http::string_body>& res);


    // URI에 따른 핸들러 등록
    void register_uri_handlers();

    // HTTP 메소드와 URI를 매핑하는 맵
    // std::unordered_map<std::string, std::function<void(const http::request<http::string_body>&, http::response<http::string_body>&)>> uri_handlers_;
    std::map<std::pair<std::string, std::string>, handler_function> uri_handlers_;  // {메소드, URI}로 핸들러 매핑

    boost::asio::io_context ioc_;
    ssl::context ssl_context_;
    tcp::acceptor acceptor_;
};

#endif // SERVER_H
