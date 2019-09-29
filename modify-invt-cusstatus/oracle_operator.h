#pragma once
#include <string>
#include <functional>
#include <occi.h>

//typedef void* (*oracleCallBack)(void* para);
typedef std::function<void* (void*)> oracleCallBack;

class OracleOperator
{
private:
	oracle::occi::Environment* env;
	oracle::occi::Connection* conn;
	oracle::occi::Statement* stmt;
	
public:
	OracleOperator(std::string user, std::string password, std::string dburl);
	~OracleOperator();
	void* select(const std::string& sql, oracleCallBack callBack);
	unsigned int executeUpdate(const std::string& sql = "");
};
