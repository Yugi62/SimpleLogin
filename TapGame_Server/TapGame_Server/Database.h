#pragma once

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/prepared_statement.h>

class Database
{
private:
	sql::mysql::MySQL_Driver* driver;
	sql::Connection* con;
	bool isInit = false;


private:
	Database() {}
	~Database() {}


public:
	static Database& GetInstance()
	{
		static Database instance;
		return instance;
	}

	void Init();
	std::string GetPassword(std::string user_id);
};