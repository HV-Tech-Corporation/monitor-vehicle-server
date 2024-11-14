# 기본 컴파일러와 플래그 설정
CXX = g++
CXXFLAGS = -std=c++17 -Wall
LIBS = -lboost_system -lboost_thread -lssl -lcrypto -pthread

# 소스 파일과 오브젝트 파일 설정
SRC = bb_server.cpp main2.cpp
OBJ = $(SRC:.cpp=.o)

# 기본 실행 파일 이름은 'server'로 설정
TARGET ?= server

# 실행 파일 빌드 규칙
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LIBS)

# 오브젝트 파일 생성 규칙
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 청소 규칙: 오브젝트 파일과 실행 파일 삭제
clean:
	rm -f $(OBJ) $(TARGET)

# 디폴트 타겟
all: $(TARGET)
