#include "bb_server.h"

// 서버 인스턴스를 반환하는 싱글톤 패턴
Server* Server::instance() {
    static Server* instance = new Server();
    return instance;
}

// 생성자
Server::Server()
    : ssl_context_(ssl::context::tlsv12_server),
      acceptor_(ioc_, {boost::asio::ip::make_address("0.0.0.0"), 8080}) {

    // SSL 설정 (OpenSSL 사용)
    ssl_context_.set_options(ssl::context::default_workarounds |
                             ssl::context::no_sslv2 |
                             ssl::context::single_dh_use);

    // SSL 인증서와 개인 키 파일 설정
    ssl_context_.use_certificate_file("server.crt", ssl::context::pem);
    ssl_context_.use_private_key_file("server.key", ssl::context::pem);
}

void Server::handle_request(beast::ssl_stream<beast::tcp_stream>& stream) {
    try {
        beast::flat_buffer buffer;

        // HTTP 요청을 파싱합니다.
        http::request<http::string_body> req;
        http::read(stream, buffer, req);

        // 메소드와 URI 추출
        std::string method = req.method_string().to_string();
        std::string uri = req.target().to_string();

        // HTTP 응답을 생성합니다.
        http::response<http::string_body> res{http::status::ok, req.version()};

        // 메소드와 URI에 따른 분기 처리
        if (method == "GET") {
            if (uri == "/bestshot") {
                //handle_get_bestshot(req, res);
                res.set(http::field::server, "Beast");
                res.set(http::field::content_type, "text/plain");
                res.body() = "GET bestshot data";
                res.prepare_payload();
            } else if (uri == "/value") {
                //handle_get_value(req, res);
                res.set(http::field::server, "Beast");
                res.set(http::field::content_type, "text/plain");
                res.body() = "GET value data";
                res.prepare_payload();
            } else if (uri == "/ocr") {
                //handle_get_ocr(req, res);
                res.set(http::field::server, "Beast");
                res.set(http::field::content_type, "text/plain");
                res.body() = "GET ocr data";
                res.prepare_payload();
            } else if (uri == "/dir") {
                //handle_get_dir(req, res);
                res.set(http::field::server, "Beast");
                res.set(http::field::content_type, "text/plain");
                res.body() = "GET dir data";
                res.prepare_payload();
            } else if (uri == "/play") {
                //handle_ctl_play(req, res);
                res.set(http::field::server, "Beast");
                res.set(http::field::content_type, "text/plain");
                res.body() = "GET play data";
                res.prepare_payload();
            } else if (uri == "/stop") {
                //handle_ctl_stop(req, res);
                res.set(http::field::server, "Beast");
                res.set(http::field::content_type, "text/plain");
                res.body() = "GET stop data";
                res.prepare_payload();
            } else if (uri == "/rewind") {
                //handle_ctl_rewind(req, res);
                res.set(http::field::server, "Beast");
                res.set(http::field::content_type, "text/plain");
                res.body() = "GET rewind data";
                res.prepare_payload();
            } else {
                // URI가 등록된 핸들러에 없는 경우
                res.result(http::status::not_found);
                res.body() = "GET Method: URI not found";
            }
        } else {
            // GET이 아닌 다른 메소드에 대한 처리
            res.result(http::status::method_not_allowed);
            res.body() = "Method Not Allowed";
        }

        // HTTP 응답을 클라이언트에 보냅니다.
        http::write(stream, res);
    } catch (const std::exception& e) {
        std::cerr << "Error handling request: " << e.what() << std::endl;
    }
}

// 서버 실행 함수
void Server::run() {
    try {
        while (true) {
            // 클라이언트로부터 연결을 기다립니다.
            tcp::socket socket(ioc_);
            acceptor_.accept(socket);

            // SSL 스트림을 만들어서 암호화된 통신을 설정합니다.
            beast::ssl_stream<beast::tcp_stream> stream(std::move(socket), ssl_context_);

            // SSL 핸드쉐이크 (클라이언트와 SSL/TLS 연결을 설정)
            stream.handshake(ssl::stream_base::server);

            // 클라이언트의 HTTP 요청을 처리합니다.
            handle_request(stream);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
