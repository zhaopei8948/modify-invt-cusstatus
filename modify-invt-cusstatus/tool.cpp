#include "tool.h"
#include <iostream>
#include <cstdio>
#include <ctime>

std::string WinTools::sm_srcdir = "";
toolCallBack WinTools::sm_callBack = nullptr;
OracleOperator* WinTools::sm_orclOper = nullptr;

void WinTools::iteratorDir(const std::string subdir)
{
	HANDLE hFind;
	WIN32_FIND_DATA findData;

	std::string dirNew = WinTools::sm_srcdir + "\\" + subdir;
	std::string dirSearch = dirNew + "\\*.*";
	hFind = FindFirstFile(dirSearch.c_str(), &findData);
	if (hFind == INVALID_HANDLE_VALUE) {
		std::cout << "Failed to find first file!\n";
		return;
	}

	do {
		if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0)
			continue;
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			std::cout << "dir not handle\n";
			continue;
		}
		else {
			std::cout << "find file is: " << findData.cFileName << "\n";
			WinTools::sm_callBack(dirNew, findData.cFileName, &findData, nullptr);
		}
	} while (FindNextFile(hFind, &findData));

	FindClose(hFind);
}

WinTools::WinTools(UINT uElapse): m_uElapse(uElapse * 1000)
{
}

void WinTools::start()
{
	BOOL bRet;
	MSG msg;

	UINT timerid = SetTimer(NULL, 0, m_uElapse, [](HWND hWnd, UINT message, UINT_PTR idTimer, DWORD dwTime) {
		std::time_t today = std::time(nullptr) - 60 * 60;
		std::tm ttoday;
		std::tm* tmtoday = &ttoday;
		localtime_s(&ttoday, &today);
		char ctoday[11];
		std::snprintf(ctoday, 15, "%4d%02d%02d%02d", tmtoday->tm_year + 1900, tmtoday->tm_mon + 1, tmtoday->tm_mday,
			tmtoday->tm_hour);
		SYSTEMTIME now;
		GetLocalTime(&now);
		char ct[15];
		std::snprintf(ct, 15, "%4d%02d%02d%02d%02d%02d", now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond);
		std::cout << "start... time: [" << ct << "] subdir: [" << ctoday << "]\n";
		WinTools::iteratorDir(ctoday);
	});

	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
		if (bRet == -1) {

		}
		else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

void WinTools::setCallBack(toolCallBack callBack)
{
	sm_callBack = callBack;
}

void WinTools::setSrcdir(std::string srcdir)
{
	sm_srcdir = srcdir;
}

void WinTools::setOracOper(OracleOperator* orclOper)
{
	sm_orclOper = orclOper;
}

OracleOperator* WinTools::getOrclOper()
{
	return sm_orclOper;
}
