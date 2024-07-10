// Winsock 라이브러리 연결
#pragma comment(lib, "ws2_32.lib") 

#include <WinSock2.h> 

#include <string>
#include <iostream>
#include <thread>
#include <vector>


// MySQL 연결 헤더파일
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#define MAX_BUFF_SIZE 1024 // 데이터 공간 : 버퍼
#define PORT 7777

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;
using std::thread;

// 구조체로 변경 필요
SOCKET serverSocket;
SOCKET clientSocket;

// 클라이언트 정보 받는 함수
void handleClient(SOCKET client_socket);
void insertDataToDatabase(float data);


int main() {

	// for WSAStartup() : Windows Socket API 초기화
	WSADATA wsaData;

	int code = WSAStartup(MAKEWORD(2, 2), &wsaData);

	// 초기화 성공 : Winsock 사용준비
	if (code == 0)
	{

		// 1. 소켓 생성 - socket()
		serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);


		// 2. 생성된 소켓에 주소 부여 - bind()
		// 소켓 주소 정의 - 초기화 
		SOCKADDR_IN server_addr = {};
		// 소켓 주소 정의 - 주소 체계
		server_addr.sin_family = AF_INET;
		// 소켓 주소 정의 - 포트
		server_addr.sin_port = htons(PORT); // host to network (Big-endian 전환) 
		// 소켓 주소 정의 - IP 
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY - 어느 환경에서든 나의 IP를 의미 -> 내 컴퓨터에서 통신

		bind(serverSocket, (sockaddr*)&server_addr, sizeof(server_addr));

		// 3. 소켓 활성화 - 클라이언트 요청 기다림 - listen()
		listen(serverSocket, SOMAXCONN);

		cout << "서버가 포트에서 수신 중 입니다 (포트 : " << PORT << ')' << endl;

		// accept() 과정
		while (true)
		{

			SOCKADDR_IN client_addr = {}; // client 소켓 주소 정의 - 초기화
			int client_addrsize = sizeof(client_addr);

			// 8byte 공간 1024개 : 읽고 쓰기 위한 용도
			char buffer[MAX_BUFF_SIZE] = { };

			ZeroMemory(&client_addr, client_addrsize);

			SOCKET clientSocket = {};

			// client에서 connect() 이후 서버에 새로운 소켓 생성
			clientSocket = accept(serverSocket, (sockaddr*)&client_addr, &client_addrsize);

			// client 연결  성공 
			if (clientSocket != INVALID_SOCKET) {
				cout << "Client connected!" << endl;

				thread clientThread(handleClient, clientSocket);

				// 메인 스레드와 분리하여 독립적으로 실행
				clientThread.detach();
			}
			else {
				cout << "Client connection failed!" << endl;
			}
		}

	}

	// 초기화 실패 
	else
	{
		cout << "프로그램 종료. (Error Code: " << code << ")";
	}


	// 종료
	closesocket(serverSocket);
	WSACleanup();

	return 0;
}

void handleClient(SOCKET clientSocket) {

	// C++에서는 네트워크 소켓을 통해 받은 데이터를 char 배열로 처리
	char buffer[MAX_BUFF_SIZE];

	while (true)
	{
		// client 정보가 들어있는 소켓을 수신 : accept 대기 , client에서 send()시 recv()함
		// 실제로 받아온 데이터의 길이 저장
		int byteReceived = recv(clientSocket, buffer, MAX_BUFF_SIZE, 0);

		// 오류 case
		if (byteReceived == SOCKET_ERROR)
		{
			cout << "오류가 발생했습니다. \n";
			closesocket(clientSocket);
			break;
		}

		// 클라이언트 연결 case
		if (byteReceived == 0)
		{
			cout << "클라이언트와의 연결이 종료되었습니다.\n";
			closesocket(clientSocket);
			break;
		}

		/* C와 C++에서 문자열은 널 문자('\0')로 끝나야 합니다.
		이는 문자열의 끝을 나타내며, 문자열 함수들이 이 문자를 만날 때까지만 처리를 하도록 합니다.
		네트워크 통신에서 recv 함수를 사용하여 데이터를 받으면, 받은 데이터는 단순히 문자열로 사용하기에 앞서 널 문자('\0')로 끝나지 않습니다.
		따라서 받은 데이터의 끝에 직접 널 문자('\0')을 추가해줘야만 C 스타일 문자열 함수들이 이를 올바르게 처리할 수 있습니다.
		*/

		buffer[byteReceived] = '\0';
		cout << "Received message : " << buffer << endl;

		// float 변환
		float buffer_f = std::stof(buffer);

		// DB에 저장
		insertDataToDatabase(buffer_f);

		// 클라이언트에게 응답 전송 C# - String 타입으로 텍스트 데이터 표현 
		string response = "Login successful!";
		send(clientSocket, response.c_str(), response.size(), 0);

	}
}

// 디비 저장
void insertDataToDatabase(float data) {
	try {
		sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
		std::unique_ptr<sql::Connection> con(driver->connect("tcp://127.0.0.1:3306", "user", "1234"));
		con->setSchema("AcCumsum");

		std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("INSERT INTO Power (aircon_p, refri_p) VALUES (?,?)"));
		pstmt->setDouble(1, data);
		pstmt->setDouble(2, data);
		pstmt->executeUpdate();
	}
	catch (sql::SQLException& e) {
		std::cerr << "MySQL error: " << e.what() << std::endl;
	}
}



