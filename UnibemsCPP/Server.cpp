// Winsock ���̺귯�� ����
#pragma comment(lib, "ws2_32.lib") 

#include <WinSock2.h> 
#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <sstream>

// MySQL ���� �������
#include "Database_insert.h"
#include "DataPars.h"

#define MAX_BUFF_SIZE 1024 // ������ ���� : ����
#define PORT 7777

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;
using std::thread;

// ����ü�� ���� �ʿ�
SOCKET serverSocket;
Database db;

// Ŭ���̾�Ʈ ���� �޴� �Լ�
void handleClient(SOCKET client_socket);

int main() {
    // for WSAStartup() : Windows Socket API �ʱ�ȭ
    WSADATA wsaData;

    int code = WSAStartup(MAKEWORD(2, 2), &wsaData);

    // �ʱ�ȭ ���� : Winsock ����غ�
    if (code == 0) {
        // 1. ���� ���� - socket()
        serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // AF_INET ��� Ȯ��

        if (serverSocket == INVALID_SOCKET) {
            cout << "Socket creation failed with error: " << WSAGetLastError() << endl;
            WSACleanup();
            return 1;
        }

        // 2. ������ ���Ͽ� �ּ� �ο� - bind()
        // ���� �ּ� ���� - �ʱ�ȭ 
        SOCKADDR_IN server_addr = {};
        // ���� �ּ� ���� - �ּ� ü��
        server_addr.sin_family = AF_INET;
        // ���� �ּ� ���� - ��Ʈ
        server_addr.sin_port = htons(PORT); // host to network (Big-endian ��ȯ) 
        // ���� �ּ� ���� - IP 
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY - ��� ȯ�濡���� ���� IP�� �ǹ� -> �� ��ǻ�Ϳ��� ���

        if (::bind(serverSocket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
            cout << "Bind failed with error: " << WSAGetLastError() << endl;
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        // 3. ���� Ȱ��ȭ - Ŭ���̾�Ʈ ��û ��ٸ� - listen()
        if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
            cout << "Listen failed with error: " << WSAGetLastError() << endl;
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        cout << "������ ��Ʈ���� ���� �� �Դϴ� (��Ʈ : " << PORT << ')' << endl;

        // accept() ����
        while (true) {
            SOCKADDR_IN client_addr = {}; // client ���� �ּ� ���� - �ʱ�ȭ
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
                cout << "DB ���� ����" << endl;
                closesocket(clientSocket);
                continue; // ���� Ŭ���̾�Ʈ ������ ��ٸ��ϴ�.
            }

            thread clientThread(handleClient, clientSocket);
            clientThread.detach();
        }
        // DB ���� ����
        db.disconnect();
    }
    else {
        cout << "���α׷� ����. (Error Code: " << code << ")";
    }

    // ����
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}

void handleClient(SOCKET clientSocket) {
    char buffer[MAX_BUFF_SIZE + 1]; // +1 to ensure null termination

    while (true) {
        int byteReceived = recv(clientSocket, buffer, MAX_BUFF_SIZE, 0);

        if (byteReceived == SOCKET_ERROR) {
            cout << "������ �߻��߽��ϴ�. \n";
            closesocket(clientSocket);
            break;
        }

        if (byteReceived == 0) {
            cout << "Ŭ���̾�Ʈ���� ������ ����Ǿ����ϴ�.\n";
            closesocket(clientSocket);
            break;
        }

        buffer[byteReceived] = '\0'; // �� ���� �߰�
        cout << "Received message : " << buffer << endl;

        vector<string> parsedData = parseData(buffer);

        for (const auto& dataPart : parsedData)
        {
            vector<float> data;
            string table;

            if (parseSensorData(dataPart, data, table))
            {
                if (table == "Sensor")
                {
                    if (data.size() == 4)
                    {
                        db.insertSensor(static_cast<int>(data[0]), data[1], data[2], data[3]);
                    }
                }
                
                else if (table == "HAVC")
                {
                    if (data.size() == 3)
                    {
                        db.insertHAVC(static_cast<int>(data[0]), data[1], data[2]);
                    }
                }

                else if (table == "Ventil")
                {
                    if (data.size() == 3)
                    {
                        db.insertVentil(static_cast<int>(data[0]), data[1], data[2]);
                    }
                }

                else if (table == "EV")
                {
                    if (data.size() == 2)
                    {
                        db.insertEV(static_cast<int>(data[0]), data[1]);
                    }
                }

                else if (table == "ES")
                {
                    if (data.size() == 2)
                    {
                        db.insertES(static_cast<int>(data[0]), data[1]);
                    }
                }
            }
        }

        // ���� ������ Ŭ���̾�Ʈ�� ����
        // ��ú���
        string avgSensorData = db.getAvgSensorData();
        string totalPower = db.getTotalPower();

        // ��ġid �ش�Ǵ� ���� �� (������)
        string havc1dataview = db.gethavc1dataview();
        string havc2dataview = db.gethavc2dataview();
        string havc3dataview = db.gethavc3dataview();
        string havc4dataview = db.gethavc4dataview();
;
        // ��ġid �ش�Ǵ� ���� �� (����)
        string sensor1dataview = db.getsensor1dataview();
        string sensor2dataview = db.getsensor2dataview();
        string sensor3dataview = db.getsensor3dataview();

        // ��ġid �ش�Ǵ� ���� �� (��ƿ)
        string ventil1dataview = db.getventil1dataview();
        string ventil2dataview = db.getventil2dataview();
        string ventil3dataview = db.getventil3dataview();

        // ��ġid �ش�Ǵ� ���� �� (����������, �����÷�����)
        string ev1dataview = db.getev1dataview();
        string es1dataview = db.getes1dataview();

        // Ŭ���̾�Ʈ���� ���� ���� C# - String Ÿ������ �ؽ�Ʈ ������ ǥ�� 
        std::string response = "Login successful!";
        send(clientSocket, response.c_str(), static_cast<int>(response.size()), 0);
    }
}
