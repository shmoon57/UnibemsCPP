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
	Database(); // 생성 및 소멸자
	~Database();

	bool connect(const database_info& condb);
	void disconnect();
	void insertHAVC(int h_id, float h_power, float h_set_temp, int status);
	void insertSensor(int s_id, float s_temp, float s_humidity, float s_co2);
	void insertVentil(int v_id, float v_wind, float v_power);
	void insertEV(int ev_id, float ev_power);
	void insertES(int es_id, float es_power);
	void insertVentil_control(int vc_id, int d_id, int dvalue);
	
	// 대시보드 출력 뷰
	string getAvgSensorData();
	string getTotalPower();

	// 위치id 해당되는 설비별 뷰 (에어컨)
	string gethavc1dataview();
	string gethavc2dataview();
	string gethavc3dataview();
	string gethavc4dataview();

	// 위치id 해당되는 설비별 뷰 (센서)
	string getsensor1dataview();
	string getsensor2dataview();
	string getsensor3dataview();

	// 위치id 해당되는 설비별 뷰 (벤틸)
	string getventil1dataview();
	string getventil2dataview();
	string getventil3dataview();

	// 위치id 해당되는 설비별 뷰 (엘리베이터, 에스컬레이터)
	string getev1dataview();
	string getes1dataview();

private:
	sql::Driver* driver;
	unique_ptr<sql::Connection> con;
};


#endif // !DATABASE.H
