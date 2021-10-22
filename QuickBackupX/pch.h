#ifndef PCH_H
#define PCH_H
#define WIN32_LEAN_AND_MEAN
// Windows 头文件
#include <windows.h>
// C++ 标准库
#include <string>
#include <string_view>
#include <map>
#include <vector>
#include <filesystem>
#include <atomic>
#include <thread>

#include <iostream>
#include <fstream>
#include <sstream>

#include <cstdio>
#include <ctime>
// LiteLoader
#include <liteloader.h>

#define FETCH(tp, ptr) (*reinterpret_cast<tp*>(ptr))
#define PRINT coutp.p
#define LOG loutp.p
#define BDS_V1_16
//#define BDS_V1_17
#undef max
#undef min
#undef ERROR
#if defined(BDS_V1_16)
	#pragma comment(lib,"./LLSDK_1.16/LiteLoader.lib")
#elif defined(BDS_V1_17_2)
	#pragma comment(lib,"./LLSDK_1.17/LiteLoader.lib")
#endif

using VA = unsigned long long;
using RVA = unsigned int; 
typedef unsigned long long xuid_t;
typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;

template <typename COMMITER>
class Logger;
struct stdio_commit;
struct file_commit;
namespace QuickBackupX
{
	namespace fs = std::filesystem;
	// 类声明
	class Backup;
	// 外部变量
	extern Logger<stdio_commit*> coutp;
	extern Logger<file_commit*> loutp;

	const std::string QBX_VERSION = "5.0.0";
	const std::string QBX_VERTYPE = "Beta";
	const std::string QBX_FOLDER  = "./plugins/QuickBackupX";
	const std::string QBX_CONFIG  = "./plugins/QuickBackupX/config.json";
	const std::string QBX_RECORD  = "./plugins/QuickBackupX/data.db";
	const std::string QBX_LOGFILE = "./plugins/QuickBackupX/qbx.log";
}

#endif //PCH_H
