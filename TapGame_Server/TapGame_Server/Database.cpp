#include "Database.h"

void Database::Init()
{
	if (!isInit)
	{
		try
		{
			driver = sql::mysql::get_mysql_driver_instance();
			con = driver->connect("tcp://127.0.0.1:3306", "root", "0000");
			con->setSchema("game");
		}
		catch (sql::SQLException& error)
		{
			//cout << e.what() << endl;
		}
	}
}

std::string Database::GetPassword(std::string user_id)
{
	//파라미터 바인딩 (sql injection 방지)
	sql::PreparedStatement* ps = con->prepareStatement("select user_password from users where user_id = ?");
	ps->setString(1, user_id);
	auto result = ps->executeQuery();

	std::string str = "";

	while (result->next())
		str = result->getString("user_password");

	return str;
}
