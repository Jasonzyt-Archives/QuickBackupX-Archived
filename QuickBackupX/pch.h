// Created by twoone3
#pragma once
#ifndef PCH_INCLUDED
#define WIN32_LEAN_AND_MEAN
#define PCH_INCLUDED
// Windows 头文件
#include <windows.h>
// C++ 标准库
#include <iostream>
#include <thread>
#include <string>
#include <filesystem>
#include <fstream>
#include <map>
// 其他 API 头文件
#include <stdio.h>
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")

using VA = unsigned __int64;
using RVA = unsigned int;
template<typename Type>
using Ptr = Type*;

namespace QuickBackupX
{
	// 类/结构声明
	class BRecJson;
	class Config;
	class Logger;
	struct Player;
	// 外部变量
	extern BRecJson* rec;
	extern Config* cfg;
	extern Logger* log;
	extern VA pxuid_level;
	extern std::map<Player*, bool> PlayerOnline;
	extern std::map<std::string, Player*> PlayerUuid;
	extern std::map<std::string, bool> PlayerIsOnline;
	extern std::map<unsigned, bool> fids;
	// 常量定义
	const std::string QBXVERSIONTAG = "Beta";
	const std::string QBXVERSION    = "4.1.0";
	const std::string TEMPDIR       = "./QuickBackupX/LEVEL_TEMP/";
	const std::string RESUMEDIR     = "./QuickBackupX/RESUME_TEMP/";
	const std::string EULAFILE      = "./QuickBackupX/EULA.txt";
	const std::string LOGFILE       = "./QuickBackupX/QBX.log";
	const std::string CONFIGFILE    = "./QuickBackupX/config.json";
	const std::string BACKUPRECFILE = "./QuickBackupX/backups.json";
}
namespace QBX = QuickBackupX;

#define SYM(sym) GetServerSymbol(sym)

extern "C" {
	_declspec(dllimport) int HookFunction(void*, void*, void*);
	_declspec(dllimport) void* GetServerSymbol(const char*);
}
template<typename ret = void, typename... Args>
static inline ret SYMCALL(const char* sym, Args... args) {
	return ((ret(*)(Args...))SYM(sym))(args...);
}
static inline void* HookRegister(const char* sym, void* hook, void* org) {
	void* found = SYM(sym);
	if (!found)printf("Failed to hook %s\n", sym);
	HookFunction(found, org, hook);
	return org;
}
#define SYMHOOK(name, ret, sym, ...)		\
namespace name {						\
	typedef ret(*fn)(__VA_ARGS__);		\
	static ret _hook(__VA_ARGS__);		\
	static fn original = *(fn*)HookRegister(sym, name::_hook, &name::original);\
};										\
ret name::_hook(__VA_ARGS__)

#define _U(x) u8 ## x
#define offset(type, ptr) (*reinterpret_cast<type*>(ptr))
#define Debug if (QBX::cfg->debug)
//#define offset(type,ptr) (*(type*)(ptr))

#endif