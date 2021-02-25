// Created by JasonZYT on 2021/02/05
#pragma once
#ifndef QBX_FUNC_INCLUDED
#define QBX_FUNC_INCLUDED
#include "pch.h"
#include "md5.h"
#include "json/json.h"
#include <time.h>

namespace QuickBackupX
{
	// 时间相关操作
	std::string getTime();
	std::string getCustomTime(const char* Format);
	// 文件相关操作
	size_t getFileSize(std::string fn);
	std::string getLevelName();
	// 数据相关操作
	wchar_t* CharToWChar(const char* str);
	std::vector<std::string> split(std::string s, const char flag = ' ');
	float round(float x, int pre);
	double round(double x, int pre);
	std::string round_str(double x, int pre);
	std::string SizeToString(size_t size);
	std::string to_UTF8(const std::string& str);
	std::string ZipRetCheck(int in);
	// Json相关操作
	Json::Value getJSON(std::string fn);
	bool SWriteIntoFile(Json::Value root, std::string fn);
	// 游戏相关操作
	bool runcmd(std::string cmd);
	bool sendText(std::string playername, std::string text);
	std::string getVersion();
	static unsigned getFormId();
	bool DestroyForm(unsigned fid);
	unsigned SendForm(std::string uuid, std::string str);
	bool Is_Admin(std::string pname, std::string pxuid);
	bool TellAdmin(std::string msg);
}

#endif // !QBX_FUNC_INCLUDED

