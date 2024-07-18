#include "Database_insert.h"
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>
#include <iostream>

using namespace std;

Database::Database() : driver(nullptr), con(nullptr) {}

Database::~Database()
{
	disconnect();
}

bool Database::connect(const database_info& condb)
{
	try
	{
		driver = sql::mysql::get_mysql_driver_instance();
		con.reset(driver->connect(condb.host, condb.user, condb.password));
		con->setSchema(condb.schema);
		
		return true;
	}

	catch (sql::SQLException& e)
	{
		cerr << "MySQL error: " << e.what() << endl;
		return false;
	}

}

void Database::disconnect()
{
	if (con)
	{
		con->close();
	}
}

void Database::insertHAVC(int h_id, float h_power, float h_set_temp, int status)
{
	try
	{
		if (!con)
		{
			cerr << "DB ¿¬°áÀÌ ¾ÈµÊ" << endl;
			return;
		}

		unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
			"INSERT INTO havc (h_id, h_power, h_set_temp, status, TIMESTAMP) VALUES (?, ?, ?, ?, CURRENT_TIMESTAMP) "));
			

		pstmt->setInt(1, h_id);
		pstmt->setDouble(2, h_power);
		pstmt->setDouble(3, h_set_temp);
		pstmt->setInt(4, status);
		pstmt->executeUpdate();

	}

	catch (sql::SQLException& e)
	{
		cerr << "MySQL error: " << e.what() << std::endl;

	}
}

void Database::insertSensor(int s_id, float s_temp, float s_humidity, float s_co2)
{
	try
	{
		if (!con)
		{
			cerr << "DB ¿¬°áÀÌ ¾ÈµÊ" << endl;
			return;
		}

		unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
			"INSERT INTO sensor (s_id, s_temp, s_humidity, s_co2, TIMESTAMP) VALUES (?, ?, ?, ?, CURRENT_TIMESTAMP) "));

		pstmt->setInt(1, s_id);
		pstmt->setDouble(2, s_temp);
		pstmt->setDouble(3, s_humidity);
		pstmt->setDouble(4, s_co2);
		pstmt->executeUpdate();

	}

	catch (sql::SQLException& e)
	{
		cerr << "MySQL error: " << e.what() << std::endl;

	}
}

void Database::insertVentil(int v_id, float v_wind, float v_power)
{
	try
	{
		if (!con)
		{
			cerr << "DB ¿¬°áÀÌ ¾ÈµÊ" << endl;
			return;
		}

		unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
			"INSERT INTO ventil (v_id, v_wind, v_power, TIMESTAMP) VALUES (?, ?, ?, CURRENT_TIMESTAMP) "));

		pstmt->setInt(1, v_id);
		pstmt->setDouble(2, v_wind);
		pstmt->setDouble(3, v_power);
		pstmt->executeUpdate();

	}

	catch (sql::SQLException& e)
	{
		cerr << "MySQL error: " << e.what() << std::endl;

	}
}

void Database::insertEV(int ev_id, float ev_power)
{
	try
	{
		if (!con)
		{
			cerr << "DB ¿¬°áÀÌ ¾ÈµÊ" << endl;
			return;
		}

		unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
			"INSERT INTO ev (ev_id, ev_power, TIMESTAMP) VALUES (?, ?, CURRENT_TIMESTAMP) "));

		pstmt->setInt(1, ev_id);
		pstmt->setDouble(2, ev_power);
		pstmt->executeUpdate();

	}

	catch (sql::SQLException& e)
	{
		cerr << "MySQL error: " << e.what() << std::endl;

	}
}

void Database::insertES(int es_id, float es_power)
{
	try
	{
		if (!con)
		{
			cerr << "DB ¿¬°áÀÌ ¾ÈµÊ" << endl;
			return;
		}

		unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
			"INSERT INTO es (es_id, es_power, TIMESTAMP) VALUES (?, ?, CURRENT_TIMESTAMP) "));

		pstmt->setInt(1, es_id);
		pstmt->setDouble(2, es_power);
		pstmt->executeUpdate();

	}

	catch (sql::SQLException& e)
	{
		cerr << "MySQL error: " << e.what() << std::endl;

	}
}

void Database::insertVentil_control(int vc_id, int d_id, int dvalue)
{
	try
	{
		if (!con)
		{
			cerr << "DB ¿¬°áÀÌ ¾ÈµÊ" << endl;
			return;
		}

		unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
			"INSERT INTO ventil_control (vc_id, d_id, dvalue, TIMESTAMP) VALUES (?, ?, ?, CURRENT_TIMESTAMP) "));

		pstmt->setInt(1, vc_id);
		pstmt->setInt(2, d_id);
		pstmt->setInt(3, dvalue);
		pstmt->executeUpdate();

	}

	catch (sql::SQLException& e)
	{
		cerr << "MySQL error: " << e.what() << std::endl;

	}
}

string Database::getAvgSensorData()
{
	try
	{
		if (!con)
		{
			cerr << "DB ¿¬°áÀÌ ¾ÈµÊ" << endl;
			return "";
		}

		unique_ptr<sql::Statement> stmt(con->createStatement());
		unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM unibemsdb.avg_sensor_data"));

		if (res->next())
		{
			sql::ResultSetMetaData* metaData = res->getMetaData();
			int numColumns = metaData->getColumnCount();
			for (int i = 1; i <= numColumns; ++i)
			{
				cout << "Column : " << i << " name : " << metaData->getColumnLabel(i) << endl;
			}
			string avg_temp = res->getString("avg_temp");
			string avg_humidity = res->getString("avg_humidity");
			string avg_co2 = res->getString("avg_co2");

			return "Avg_Temp : " + avg_temp + ", Avg_Humidity : " + avg_humidity + ", Avg_Co2 : " + avg_co2;
		}
		else
		{
			cerr << "No data returned" << endl;
		}
	}
	catch (sql::SQLException& e)
	{
		cerr << "MySQL error : " << e.what() << endl;
	}

	return "";
}

string Database::getTotalPower()
{
	try
	{
		if (!con)
		{
			cerr << "DB ¿¬°áÀÌ ¾ÈµÊ" << endl;
			return "";
		}

		unique_ptr<sql::Statement> stmt(con->createStatement());
		unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM unibemsdb.total_power"));

		if (res->next())
		{
			sql::ResultSetMetaData* metaData = res->getMetaData();
			int numColumns = metaData->getColumnCount();
			for (int i = 1; i <= numColumns; ++i)
			{
				cout << "Column : " << i << " name : " << metaData->getColumnLabel(i) << endl;
			}
			string total_power = res->getString("total_power");

			return "total_power : " + total_power;
		}
		else
		{
			cerr << "No data returned" << endl;
		}
	}
	catch (sql::SQLException& e)
	{
		cerr << "MySQL error : " << e.what() << endl;
	}

	return "";
}

string Database::gethavc1dataview()
{
	try
	{
		if (!con)
		{
			cerr << "DB ¿¬°áÀÌ ¾ÈµÊ" << endl;
			return "";
		}

		unique_ptr<sql::Statement> stmt(con->createStatement());
		unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM unibemsdb.havc_1_data_view"));

		if (res->next())
		{
			sql::ResultSetMetaData* metaData = res->getMetaData();
			int numColumns = metaData->getColumnCount();
			for (int i = 1; i <= numColumns; ++i)
			{
				cout << "Column : " << i << " name : " << metaData->getColumnLabel(i) << endl;
			}
			string set_temp = res->getString("set_temp");
			string power = res->getString("power");

			return "h1_Set_Temp : " + set_temp + ", h1_Power : " + power;
		}
		else
		{
			cerr << "No data returned" << endl;
		}
	}
	catch (sql::SQLException& e)
	{
		cerr << "MySQL error : " << e.what() << endl;
	}

	return "";
}

string Database::gethavc2dataview()
{
	try
	{
		if (!con)
		{
			cerr << "DB ¿¬°áÀÌ ¾ÈµÊ" << endl;
			return "";
		}

		unique_ptr<sql::Statement> stmt(con->createStatement());
		unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM unibemsdb.havc_2_data_view"));

		if (res->next())
		{
			sql::ResultSetMetaData* metaData = res->getMetaData();
			int numColumns = metaData->getColumnCount();
			for (int i = 1; i <= numColumns; ++i)
			{
				cout << "Column : " << i << " name : " << metaData->getColumnLabel(i) << endl;
			}
			string set_temp = res->getString("set_temp");
			string power = res->getString("power");

			return "h2_Set_Temp : " + set_temp + ", h2_Power : " + power;
		}
		else
		{
			cerr << "No data returned" << endl;
		}
	}
	catch (sql::SQLException& e)
	{
		cerr << "MySQL error : " << e.what() << endl;
	}

	return "";
}

string Database::gethavc3dataview()
{
	try
	{
		if (!con)
		{
			cerr << "DB ¿¬°áÀÌ ¾ÈµÊ" << endl;
			return "";
		}

		unique_ptr<sql::Statement> stmt(con->createStatement());
		unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM unibemsdb.havc_3_data_view"));

		if (res->next())
		{
			sql::ResultSetMetaData* metaData = res->getMetaData();
			int numColumns = metaData->getColumnCount();
			for (int i = 1; i <= numColumns; ++i)
			{
				cout << "Column : " << i << " name : " << metaData->getColumnLabel(i) << endl;
			}
			string set_temp = res->getString("set_temp");
			string power = res->getString("power");

			return "h3_Set_Temp : " + set_temp + ", h3_Power : " + power;
		}
		else
		{
			cerr << "No data returned" << endl;
		}
	}
	catch (sql::SQLException& e)
	{
		cerr << "MySQL error : " << e.what() << endl;
	}

	return "";
}

string Database::gethavc4dataview()
{
	try
	{
		if (!con)
		{
			cerr << "DB ¿¬°áÀÌ ¾ÈµÊ" << endl;
			return "";
		}

		unique_ptr<sql::Statement> stmt(con->createStatement());
		unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM unibemsdb.havc_4_data_view"));

		if (res->next())
		{
			sql::ResultSetMetaData* metaData = res->getMetaData();
			int numColumns = metaData->getColumnCount();
			for (int i = 1; i <= numColumns; ++i)
			{
				cout << "Column : " << i << " name : " << metaData->getColumnLabel(i) << endl;
			}
			string set_temp = res->getString("set_temp");
			string power = res->getString("power");

			return "h4_Set_Temp : " + set_temp + ", h4_Power : " + power;
		}
		else
		{
			cerr << "No data returned" << endl;
		}
	}
	catch (sql::SQLException& e)
	{
		cerr << "MySQL error : " << e.what() << endl;
	}

	return "";
}

string Database::getsensor1dataview()
{
	try
	{
		if (!con)
		{
			cerr << "DB ¿¬°áÀÌ ¾ÈµÊ" << endl;
			return "";
		}

		unique_ptr<sql::Statement> stmt(con->createStatement());
		unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM unibemsdb.sensor_1_data_view"));

		if (res->next())
		{
			sql::ResultSetMetaData* metaData = res->getMetaData();
			int numColumns = metaData->getColumnCount();
			for (int i = 1; i <= numColumns; ++i)
			{
				cout << "Column : " << i << " name : " << metaData->getColumnLabel(i) << endl;
			}
			string temp = res->getString("temperature");
			string humidity = res->getString("humidity");
			string co2 = res->getString("co2");

			return "s1_Temp : " + temp + ", s1_Humidity : " + humidity + ", s1_Co2 : " + co2;
		}
		else
		{
			cerr << "No data returned" << endl;
		}
	}
	catch (sql::SQLException& e)
	{
		cerr << "MySQL error : " << e.what() << endl;
	}

	return "";
}

string Database::getsensor2dataview()
{
	try
	{
		if (!con)
		{
			cerr << "DB ¿¬°áÀÌ ¾ÈµÊ" << endl;
			return "";
		}

		unique_ptr<sql::Statement> stmt(con->createStatement());
		unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM unibemsdb.sensor_2_data_view"));

		if (res->next())
		{
			sql::ResultSetMetaData* metaData = res->getMetaData();
			int numColumns = metaData->getColumnCount();
			for (int i = 1; i <= numColumns; ++i)
			{
				cout << "Column : " << i << " name : " << metaData->getColumnLabel(i) << endl;
			}
			string temp = res->getString("temp");
			string humidity = res->getString("humidity");
			string co2 = res->getString("co2");

			return "s2_Temp : " + temp + ", s2_Humidity : " + humidity + ", s2_Co2 : " + co2;
		}
		else
		{
			cerr << "No data returned" << endl;
		}
	}
	catch (sql::SQLException& e)
	{
		cerr << "MySQL error : " << e.what() << endl;
	}

	return "";
}

string Database::getsensor3dataview()
{
	try
	{
		if (!con)
		{
			cerr << "DB ¿¬°áÀÌ ¾ÈµÊ" << endl;
			return "";
		}

		unique_ptr<sql::Statement> stmt(con->createStatement());
		unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM unibemsdb.sensor_3_data_view"));

		if (res->next())
		{
			sql::ResultSetMetaData* metaData = res->getMetaData();
			int numColumns = metaData->getColumnCount();
			for (int i = 1; i <= numColumns; ++i)
			{
				cout << "Column : " << i << " name : " << metaData->getColumnLabel(i) << endl;
			}
			string temp = res->getString("temp");
			string humidity = res->getString("humidity");
			string co2 = res->getString("co2");

			return "s3_Temp : " + temp + ", s3_Humidity : " + humidity + ", s3_Co2 : " + co2;
		}
		else
		{
			cerr << "No data returned" << endl;
		}
	}
	catch (sql::SQLException& e)
	{
		cerr << "MySQL error : " << e.what() << endl;
	}

	return "";
}

string Database::getventil1dataview()
{
	try
	{
		if (!con)
		{
			cerr << "DB ¿¬°áÀÌ ¾ÈµÊ" << endl;
			return "";
		}

		unique_ptr<sql::Statement> stmt(con->createStatement());
		unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM unibemsdb.ventil_1_data_view"));

		if (res->next())
		{
			sql::ResultSetMetaData* metaData = res->getMetaData();
			int numColumns = metaData->getColumnCount();
			for (int i = 1; i <= numColumns; ++i)
			{
				cout << "Column : " << i << " name : " << metaData->getColumnLabel(i) << endl;
			}
			string wind = res->getString("wind");
			string power = res->getString("power");

			return "v1_Wind : " + wind + ", v1_Power : " + power;
		}
		else
		{
			cerr << "No data returned" << endl;
		}
	}
	catch (sql::SQLException& e)
	{
		cerr << "MySQL error : " << e.what() << endl;
	}

	return "";
}

string Database::getventil2dataview()
{
	try
	{
		if (!con)
		{
			cerr << "DB ¿¬°áÀÌ ¾ÈµÊ" << endl;
			return "";
		}

		unique_ptr<sql::Statement> stmt(con->createStatement());
		unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM unibemsdb.ventil_2_data_view"));

		if (res->next())
		{
			sql::ResultSetMetaData* metaData = res->getMetaData();
			int numColumns = metaData->getColumnCount();
			for (int i = 1; i <= numColumns; ++i)
			{
				cout << "Column : " << i << " name : " << metaData->getColumnLabel(i) << endl;
			}
			string wind = res->getString("wind");
			string power = res->getString("power");

			return "v2_Wind : " + wind + ", v2_Power : " + power;
		}
		else
		{
			cerr << "No data returned" << endl;
		}
	}
	catch (sql::SQLException& e)
	{
		cerr << "MySQL error : " << e.what() << endl;
	}

	return "";
}

string Database::getventil3dataview()
{
	try
	{
		if (!con)
		{
			cerr << "DB ¿¬°áÀÌ ¾ÈµÊ" << endl;
			return "";
		}

		unique_ptr<sql::Statement> stmt(con->createStatement());
		unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM unibemsdb.ventil_3_data_view"));

		if (res->next())
		{
			sql::ResultSetMetaData* metaData = res->getMetaData();
			int numColumns = metaData->getColumnCount();
			for (int i = 1; i <= numColumns; ++i)
			{
				cout << "Column : " << i << " name : " << metaData->getColumnLabel(i) << endl;
			}
			string wind = res->getString("wind");
			string power = res->getString("power");

			return "v3_Wind : " + wind + ", v3_Power : " + power;
		}
		else
		{
			cerr << "No data returned" << endl;
		}
	}
	catch (sql::SQLException& e)
	{
		cerr << "MySQL error : " << e.what() << endl;
	}

	return "";
}

string Database::getev1dataview()
{
	try
	{
		if (!con)
		{
			cerr << "DB ¿¬°áÀÌ ¾ÈµÊ" << endl;
			return "";
		}

		unique_ptr<sql::Statement> stmt(con->createStatement());
		unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM unibemsdb.ev_1_data_view"));

		if (res->next())
		{
			sql::ResultSetMetaData* metaData = res->getMetaData();
			int numColumns = metaData->getColumnCount();
			for (int i = 1; i <= numColumns; ++i)
			{
				cout << "Column : " << i << " name : " << metaData->getColumnLabel(i) << endl;
			}
			string power = res->getString("power");

			return "ev1_Power : " + power;
		}
		else
		{
			cerr << "No data returned" << endl;
		}
	}
	catch (sql::SQLException& e)
	{
		cerr << "MySQL error : " << e.what() << endl;
	}

	return "";
}

string Database::getes1dataview()
{
	try
	{
		if (!con)
		{
			cerr << "DB ¿¬°áÀÌ ¾ÈµÊ" << endl;
			return "";
		}

		unique_ptr<sql::Statement> stmt(con->createStatement());
		unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM unibemsdb.es_1_data_view"));

		if (res->next())
		{
			sql::ResultSetMetaData* metaData = res->getMetaData();
			int numColumns = metaData->getColumnCount();
			for (int i = 1; i <= numColumns; ++i)
			{
				cout << "Column : " << i << " name : " << metaData->getColumnLabel(i) << endl;
			}
			string power = res->getString("power");

			return "es1_Power : " + power;
		}
		else
		{
			cerr << "No data returned" << endl;
		}
	}
	catch (sql::SQLException& e)
	{
		cerr << "MySQL error : " << e.what() << endl;
	}

	return "";
}