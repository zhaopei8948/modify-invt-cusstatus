#include "tool.h"
#include "oracle_operator.h"
#include <iostream>
#include <fstream>
#include "tinyxml2.h"

std::string getTodayLogFileName()
{
	SYSTEMTIME now;
	GetLocalTime(&now);
	char ct[18];
	std::snprintf(ct, 18, "logs\\%4d%02d%02d.log", now.wYear, now.wMonth, now.wDay);
	return ct;
}

std::string getCurrentTime()
{
	SYSTEMTIME now;
	GetLocalTime(&now);
	char ct[24];
	std::snprintf(ct, 24, "%4d-%02d-%02d %02d:%02d:%02d.%03d", now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond, now.wMilliseconds);
	return ct;
}

int main(int argc, char* argv[])
{
	if (argc < 6) {
		std::cout << "usage:" << argv[0] << " <user> <password> <dburl> <srcdir> <interval>\n";
		exit(1);
	}
	std::string user = argv[1];
	std::string password = argv[2];
	std::string dburl = argv[3];
	std::string srcdir = argv[4];
	int interval = std::atoi(argv[5]);
	OracleOperator *oracleOper = new OracleOperator(user, password, dburl);
	/*oracleOper->select("select login_name from user_user", [user](void* rs) -> void* {
		oracle::occi::ResultSet* resultSet = (oracle::occi::ResultSet*) rs;
		while (resultSet->next()) {
			std::cout << user << "--login_name: " << resultSet->getString(1) << std:: endl;
		}
		return nullptr;
	});*/

	std::string preLogFileName = getTodayLogFileName();
	std::ofstream logFile = std::ofstream(getTodayLogFileName());
	WinTools::setSrcdir(srcdir);
	WinTools::setCallBack([&preLogFileName, &logFile, &oracleOper](const std::string srcdir, const std::string filename, const WIN32_FIND_DATA* findData, void* para) {
		//std::cout << "filename: [" << filename << "]\n";
		tinyxml2::XMLDocument doc;
		std::string path = srcdir + "\\" + filename;
		doc.LoadFile(path.c_str());
		tinyxml2::XMLElement* rootElement = doc.FirstChildElement("InventoryStatus");
		if (rootElement == nullptr) {
			std::cout << "not input invt, not handle!!!\n";
			return;
		}
		std::string ebcCode(rootElement->FirstChildElement("ebcCode")->GetText());
		std::string copNo(rootElement->FirstChildElement("copNo")->GetText());
		std::string newInvtNo(rootElement->FirstChildElement("invtNo")->GetText());
		std::string cusStatus(rootElement->FirstChildElement("returnStatus")->GetText());
		std::cout << "ebcCode: [" << ebcCode << "] copNo: [" << copNo << "] cusStatus: [" << cusStatus << "]\n";
		oracleOper->select("select t.app_status, t.invt_no from ceb2_invt_head t where t.ebc_code = '"
			+ ebcCode + "' and t.cop_no = '" + copNo + "'", [&preLogFileName, &logFile, &ebcCode, &copNo, &cusStatus, &newInvtNo, &oracleOper](void* rs) -> void* {
			oracle::occi::ResultSet* resultSet = (oracle::occi::ResultSet*) rs;
			if (resultSet->next()) {
				std::string appStatus = resultSet->getString(1);
				std::string invtNo = resultSet->getString(2);
				std::cout << "ent-> ebcCode: [" << ebcCode << "] copNo: [" << copNo << "] appStatus: [" << appStatus
					<< "] invtNo: [" << invtNo << "]\n";
				if (invtNo == "" && appStatus != "100" && appStatus != "800" && appStatus != "899" && cusStatus == "26") {
					std::string updateInvt = "update ceb2_invt_head t set t.invt_no = '" + newInvtNo + "', t.app_status='800' where t.ebc_code = '" + ebcCode
						+ "' and t.cop_no = '" + copNo + "'";
					oracleOper->executeUpdate(updateInvt);
					std::string newFileName = getTodayLogFileName();
					if (newFileName != preLogFileName) {
						logFile.close();
						logFile = std::ofstream(newFileName);
						preLogFileName = newFileName;
					}
					logFile << "[" << getCurrentTime() << "] update ebcCode: [" << ebcCode << "] copNo: [" << copNo << "] appStatus: [800] invtNo: [" << newInvtNo << "] success!\n";
					logFile.flush();
				}
				return nullptr;
			}
			return nullptr;
		});
	});
	WinTools::setOracOper(oracleOper);
	WinTools tool = WinTools(interval);
	tool.start();

	delete oracleOper;
	logFile.close();
	return 0;
}