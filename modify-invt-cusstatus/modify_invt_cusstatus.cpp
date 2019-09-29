#include "tool.h"
#include "oracle_operator.h"
#include <iostream>
#include "tinyxml2.h"

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

	WinTools::setSrcdir(srcdir);
	WinTools::setCallBack([&oracleOper](const std::string srcdir, const std::string filename, const WIN32_FIND_DATA* findData, void* para) {
		//std::cout << "filename: [" << filename << "]\n";
		tinyxml2::XMLDocument doc;
		std::string path = srcdir + "\\" + filename;
		doc.LoadFile(path.c_str());
		tinyxml2::XMLElement* rootElement = doc.FirstChildElement("InventoryStatus");
		std::string ebcCode(rootElement->FirstChildElement("ebcCode")->GetText());
		std::string copNo(rootElement->FirstChildElement("copNo")->GetText());
		std::string cusStatus(rootElement->FirstChildElement("returnStatus")->GetText());
		std::cout << "ebcCode: [" << ebcCode << "] copNo: [" << copNo << "] cusStatus: [" << cusStatus << "]\n";
		oracleOper->select("select t.app_status, t.invt_no from ceb2_invt_head t where t.ebc_code = '"
			+ ebcCode + "' and t.cop_no = '" + copNo + "'", [&ebcCode, &copNo, &cusStatus, &oracleOper](void* rs) -> void* {
			oracle::occi::ResultSet* resultSet = (oracle::occi::ResultSet*) rs;
			if (resultSet->next()) {
				std::string appStatus = resultSet->getString(1);
				std::string invtNo = resultSet->getString(2);
				std::cout << "ebcCode: [" << ebcCode << "] copNo: [" << copNo << "] appStatus: [" << appStatus
					<< "] invtNo: [" << invtNo << "]\n";
				if (invtNo == "" && appStatus != "100" && appStatus != "800" && appStatus != "899" && cusStatus == "26") {
					std::string updateInvt = "update ceb2_invt_head t set t.invt_no = '" + invtNo + "', t.app_status='800' where t.ebc_code = '" + ebcCode
						+ "' and t.cop_no = '" + copNo + "'";
					oracleOper->executeUpdate(updateInvt);
					std::cout << "update ebcCode: [" << ebcCode << "] copNo: [" << copNo << "] appStatus: [800] invtNo: [" << invtNo << "] success!\n";
				}
				return nullptr;
			}
		});
	});
	WinTools::setOracOper(oracleOper);
	WinTools tool = WinTools(interval);
	tool.start();

	delete oracleOper;
	return 0;
}