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

    // URI 핸들러 등록
    register_uri_handlers();
}

// HTTP 메소드 처리 함수들
// this is a form that if you want to send XML data
// /*
void Server::handle_get_value(const http::request<http::string_body>& req, http::response<http::string_body>& res) {
    // 서버 정보와 응답 유형을 설정
    res.set(http::field::server, "Beast");
    res.set(http::field::content_type, "application/xml");  // XML 형식으로 응답

    // XML 데이터 본문 설정
    std::string xml_response = R"(
<response>
    <status>success</status>
    <message>GET value data</message>
    <data>
        <value>42</value>
        <description>Example value</description>
    </data>
</response>
)";

    // XML 데이터를 응답 본문에 삽입
    res.body() = xml_response;
    res.prepare_payload();  // 본문 크기와 관련된 준비

    // 이 부분에서 HTTP 응답이 완성되어 클라이언트로 전송됩니다.
}
// */

// GET METHOD handlers
void Server::handle_get_bestshot(const http::request<http::string_body>& req, http::response<http::string_body>& res) {
    res.set(http::field::server, "Beast");
    res.set(http::field::content_type, "text/plain");
    res.body() = "GET bestshot data";
    res.prepare_payload();
}
/*
void Server::handle_get_value(const http::request<http::string_body>& req, http::response<http::string_body>& res) {
    res.set(http::field::server, "Beast");
    res.set(http::field::content_type, "text/plain");
    res.body() = "GET value data";
    res.prepare_payload();
}
*/

void Server::handle_get_ocr(const http::request<http::string_body>& req, http::response<http::string_body>& res) {
    res.set(http::field::server, "Beast");
    res.set(http::field::content_type, "text/plain");
    res.body() = "GET OCR data";
    res.prepare_payload();
}

void Server::handle_get_dir(const http::request<http::string_body>& req, http::response<http::string_body>& res) {
    res.set(http::field::server, "Beast");
    res.set(http::field::content_type, "text/plain");
    res.body() = "GET dir data";
    res.prepare_payload();
}

// CTL METHOD handlers
void Server::handle_ctl_play(const http::request<http::string_body>& req, http::response<http::string_body>& res) {
    res.set(http::field::server, "Beast");
    res.set(http::field::content_type, "text/plain");
    res.body() = "CTL play command received";
    res.prepare_payload();
}

void Server::handle_ctl_stop(const http::request<http::string_body>& req, http::response<http::string_body>& res) {
    res.set(http::field::server, "Beast");
    res.set(http::field::content_type, "text/plain");
    res.body() = "CTL stop command received";
    res.prepare_payload();
}

void Server::handle_ctl_rewind(const http::request<http::string_body>& req, http::response<http::string_body>& res) {
    res.set(http::field::server, "Beast");
    res.set(http::field::content_type, "text/plain");
    res.body() = "CTL rewind command received";
    res.prepare_payload();
}


// URI에 따른 핸들러 등록
void Server::register_uri_handlers() {
   // CTL 메소드에 대한 핸들러 등록
    uri_handlers_[{"CTL", "/play"}] = [this](const http::request<http::string_body>& req, http::response<http::string_body>& res) {
        handle_ctl_play(req, res);
    };
    uri_handlers_[{"CTL", "/stop"}] = [this](const http::request<http::string_body>& req, http::response<http::string_body>& res) {
        handle_ctl_stop(req, res);
    };
    uri_handlers_[{"CTL", "/rewind"}] = [this](const http::request<http::string_body>& req, http::response<http::string_body>& res) {
        handle_ctl_rewind(req, res);
    };

    // GET 메소드에 대한 핸들러 등록
    uri_handlers_[{"GET", "/bestshot"}] = [this](const http::request<http::string_body>& req, http::response<http::string_body>& res) {
        handle_get_bestshot(req, res);
    };
    uri_handlers_[{"GET", "/value"}] = [this](const http::request<http::string_body>& req, http::response<http::string_body>& res) {
        handle_get_value(req, res);
    };
    uri_handlers_[{"GET", "/ocr"}] = [this](const http::request<http::string_body>& req, http::response<http::string_body>& res) {
        handle_get_ocr(req, res);
    };
    uri_handlers_[{"GET", "/dir"}] = [this](const http::request<http::string_body>& req, http::response<http::string_body>& res) {
        handle_get_ocr(req, res);
    };    
}

// HTTP 요청을 처리하는 함수
void Server::handle_request(beast::ssl_stream<beast::tcp_stream>& stream) {
    try {
        beast::flat_buffer buffer;

        // HTTP 요청을 파싱합니다.
        http::request<http::string_body> req;
        http::read(stream, buffer, req);

        // HTTP 응답을 생성합니다.
        http::response<http::string_body> res{
            http::status::ok, req.version()};
        
        // {메소드, URI}에 따른 핸들러 검색
        auto handler = uri_handlers_.find({req.method_string().to_string(), req.target().to_string()});
        
        if (handler != uri_handlers_.end()) {
            handler->second(req, res);  // 해당 핸들러 호출
        } else {
            res.result(http::status::not_found);
            res.body() = "URI or Method not found";
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
