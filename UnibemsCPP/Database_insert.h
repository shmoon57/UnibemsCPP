#pragma once
#ifndef DATABASE_H
#define DATABASE_H

#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <string>

using namespace std;

struct database_info
{
	string host;
	string user;
	string password;
	string schema;
};


class Database
{
public :
	Database(); // ���� �� �Ҹ���
	~Database();

	bool connect(const database_info& condb);
	void disconnect();
	void insertHAVC(int h_id, float h_power, float h_temp);
	void insertSensor(int s_id, float s_temp, float s_humidity, float s_co2);
	void insertVentil(int v_id, float v_wind, float v_power);
	void insertEV(int ev_id, float ev_power);
	void insertES(int es_id, float es_power);
	
	// ��ú��� ��� ��
	string getAvgSensorData();
	string getTotalPower();

	// ��ġid �ش�Ǵ� ���� �� (������)
	string gethavc1dataview();
	string gethavc2dataview();
	string gethavc3dataview();
	string gethavc4dataview();

	// ��ġid �ش�Ǵ� ���� �� (����)
	string getsensor1dataview();
	string getsensor2dataview();
	string getsensor3dataview();

	// ��ġid �ش�Ǵ� ���� �� (��ƿ)
	string getventil1dataview();
	string getventil2dataview();
	string getventil3dataview();

	// ��ġid �ش�Ǵ� ���� �� (����������, �����÷�����)
	string getev1dataview();
	string getes1dataview();

private:
	sql::Driver* driver;
	unique_ptr<sql::Connection> con;
};


#endif // !DATABASE.H
