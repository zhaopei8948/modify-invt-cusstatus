#include <iostream>
#include "oracle_operator.h"

OracleOperator::OracleOperator(std::string user, std::string password, std::string dburl)
{
	env = oracle::occi::Environment::createEnvironment(oracle::occi::Environment::DEFAULT);
	try {
		conn = env->createConnection(user, password, dburl);
	} catch (oracle::occi::SQLException& ex) {
		std::cout << "code: " << ex.getErrorCode() << " message: " << ex.getMessage() << std::endl;
	}
}

OracleOperator::~OracleOperator()
{
	env->terminateConnection(conn);
	oracle::occi::Environment::terminateEnvironment(env);
}

void* OracleOperator::select(const std::string& sql, oracleCallBack callBack)
{
	void* result = nullptr;
	try {
		stmt = conn->createStatement(sql);
		oracle::occi::ResultSet* resultSet = stmt->executeQuery();
		if (callBack != nullptr) {
			result = callBack(resultSet);
		}
		stmt->closeResultSet(resultSet);
		conn->terminateStatement(stmt);
	} catch (oracle::occi::SQLException& ex) {
		std::cout << "code: " << ex.getErrorCode() << " message: " << ex.getMessage() << std::endl;
	}
	return result;
}

unsigned int OracleOperator::executeUpdate(const std::string& sql)
{
	unsigned int result;
	stmt = conn->createStatement();
	result = stmt->executeUpdate(sql);
	conn->terminateStatement(stmt);
	return result;
}

