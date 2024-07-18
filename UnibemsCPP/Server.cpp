// Winsock 라이브러리 연결
#pragma comment(lib, "ws2_32.lib") 

#include <WinSock2.h> 
#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <sstream>

// MySQL 연결 헤더파일
#include "Database_insert.h"
#include "DataPars.h"

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
Database db;

// 클라이언트 정보 받는 함수
void handleClient(SOCKET client_socket);

int main() {
    // for WSAStartup() : Windows Socket API 초기화
    WSADATA wsaData;

    int code = WSAStartup(MAKEWORD(2, 2), &wsaData);

    // 초기화 성공 : Winsock 사용준비
    if (code == 0) {
        // 1. 소켓 생성 - socket()
        serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // AF_INET 사용 확인

        if (serverSocket == INVALID_SOCKET) {
            cout << "Socket creation failed with error: " << WSAGetLastError() << endl;
            WSACleanup();
            return 1;
        }

        // 2. 생성된 소켓에 주소 부여 - bind()
        // 소켓 주소 정의 - 초기화 
        SOCKADDR_IN server_addr = {};
        // 소켓 주소 정의 - 주소 체계
        server_addr.sin_family = AF_INET;
        // 소켓 주소 정의 - 포트
        server_addr.sin_port = htons(PORT); // host to network (Big-endian 전환) 
        // 소켓 주소 정의 - IP 
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY - 어느 환경에서든 나의 IP를 의미 -> 내 컴퓨터에서 통신

        if (::bind(serverSocket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
            cout << "Bind failed with error: " << WSAGetLastError() << endl;
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        // 3. 소켓 활성화 - 클라이언트 요청 기다림 - listen()
        if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
            cout << "Listen failed with error: " << WSAGetLastError() << endl;
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        cout << "서버가 포트에서 수신 중 입니다 (포트 : " << PORT << ')' << endl;

        // accept() 과정
        while (true) {
            SOCKADDR_IN client_addr = {}; // client 소켓 주소 정의 - 초기화
            int client_addrsize = sizeof(client_addr);

            ZeroMemory(&client_addr, client_addrsize);

            SOCKET clientSocket = accept(serverSocket, (sockaddr*)&client_addr, &client_addrsize);

            if (clientSocket == INVALID_SOCKET) {
                cout << "Client connection failed with error: " << WSAGetLastError() << endl;
                continue;
            }

            cout << "Client connected!" << endl;

            database_info dbinfo = { "tcp://127.0.0.1:3306", "user", "1234","unibemsdb"};

            if (!db.connect(dbinfo)) {
                cout << "DB 연결 실패" << endl;
                closesocket(clientSocket);
                continue; // 다음 클라이언트 연결을 기다립니다.
            }

            thread clientThread(handleClient, clientSocket);
            clientThread.detach();
        }
        // DB 연결 해제
        db.disconnect();
    }
    else {
        cout << "프로그램 종료. (Error Code: " << code << ")";
    }

    // 종료
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}

void handleClient(SOCKET clientSocket) {
    char buffer[MAX_BUFF_SIZE + 1]; // +1 to ensure null termination

    while (true) {
        int byteReceived = recv(clientSocket, buffer, MAX_BUFF_SIZE, 0);

        if (byteReceived == SOCKET_ERROR) {
            cout << "오류가 발생했습니다. \n";
            closesocket(clientSocket);
            break;
        }

        if (byteReceived == 0) {
            cout << "클라이언트와의 연결이 종료되었습니다.\n";
            closesocket(clientSocket);
            break;
        }

        buffer[byteReceived] = '\0'; // 널 문자 추가
        cout << "Received message : " << buffer << endl;

        vector<string> parsedData = parseData(buffer);

        for (const auto& dataPart : parsedData)
        {
            vector<float> data;
            string table;

            if (parseSensorData(dataPart, data, table))
            {
                if (table == "Sensor" && data.size() == 4)
                { 
                 db.insertSensor(static_cast<int>(data[0]), data[1], data[2], data[3]);   
                }
                else if (table == "HAVC" && data.size() == 4)
                {
                 db.insertHAVC(static_cast<int>(data[0]), data[1], data[2], data[3]);

                }

                else if (table == "VENTIL" && data.size() == 3)
                {
                 db.insertVentil(static_cast<int>(data[0]), data[1], data[2]);
                }

                else if (table == "EV" && data.size() == 2)
                {
                 db.insertEV(static_cast<int>(data[0]), data[1]);
                }

                else if (table == "ES" && data.size() == 2)
                {
                 db.insertES(static_cast<int>(data[0]), data[1]);
                }

                else if (table == "VC" && data.size() == 3)
                {
                 db.insertVentil_control(static_cast<int>(data[0]), data[1], data[2]);
                }
            }
        }

        // 가공 데이터 클라이언트로 전송
        // 대시보드
        string avgSensorData = db.getAvgSensorData();
        string totalPower = db.getTotalPower();

        // 위치id 해당되는 설비별 뷰 (에어컨)
        string havc1dataview = db.gethavc1dataview();
        string havc2dataview = db.gethavc2dataview();
        string havc3dataview = db.gethavc3dataview();
        string havc4dataview = db.gethavc4dataview();
;
        // 위치id 해당되는 설비별 뷰 (센서)
        string sensor1dataview = db.getsensor1dataview();
        string sensor2dataview = db.getsensor2dataview();
        string sensor3dataview = db.getsensor3dataview();

        // 위치id 해당되는 설비별 뷰 (벤틸)
        string ventil1dataview = db.getventil1dataview();
        string ventil2dataview = db.getventil2dataview();
        string ventil3dataview = db.getventil3dataview();

        // 위치id 해당되는 설비별 뷰 (엘리베이터, 에스컬레이터)
        string ev1dataview = db.getev1dataview();
        string es1dataview = db.getes1dataview();

        //// 빈값 0
        //vector<string> data =
        //{
        //    avgSensorData, totalPower, havc1dataview, havc2dataview, havc3dataview, havc4dataview,
        //    sensor1dataview, sensor2dataview, sensor3dataview,
        //    ventil1dataview, ventil2dataview, ventil3dataview,
        //    ev1dataview, es1dataview
        //};
        //for (int i = 0; i < data.size(); ++i) 
        //{
        //    if (data[i].empty()) 
        //    {
        //        data[i] = "0";
        //    }
        //}
        //string response;

        //for (int i = 0; i < data.size(); ++i)
        //{
        //    response += data[i];
        //    if (i < data.size() - 1)
        //    {
        //        response += ";";
        //    }
        //}
        
        // 클라이언트에게 응답 전송 C# - String 타입으로 텍스트 데이터 표현 
        //데이터를 결합하여 하나의 문자열로 만듭니다.
        string response =
            avgSensorData + ";" + totalPower + ";" + havc1dataview + ";" + havc2dataview + ";" + havc3dataview + ";" + havc4dataview + ";" + sensor1dataview + ";" + sensor2dataview + ";" + sensor3dataview + ";" +
            ventil1dataview + ";" + ventil2dataview + ";" + ventil3dataview + ";" + ev1dataview + ";" + es1dataview;

        send(clientSocket, response.c_str(), static_cast<int>(response.size()), 0);
    }
}
