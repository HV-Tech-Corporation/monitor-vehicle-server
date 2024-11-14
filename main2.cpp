#include "bb_server.h"

int main() {
    // 싱글톤 인스턴스를 통해 서버 실행
    Server::instance()->run();
    return 0;
}
