#pragma once
#include <windows.h>
#include <functional>
#include "oracle_operator.h"

//typedef void (*toolCallBack)(const std::string path, const std::string filename, WIN32_FIND_DATA* findData, void* para);
typedef std::function <void (const std::string, const std::string, const WIN32_FIND_DATA*, void*)> toolCallBack;

class WinTools {
private:
	UINT m_uElapse;
	static std::string sm_srcdir;
	static toolCallBack sm_callBack;
	static OracleOperator* sm_orclOper;
	static void iteratorDir(const std::string subdir);

public:
	WinTools(UINT uElapse);
	void start();
	static void setCallBack(toolCallBack callBack);
	static void setSrcdir(std::string srcdir);
	static void setOracOper(OracleOperator* orclOper);
	static OracleOperator* getOrclOper();
};
