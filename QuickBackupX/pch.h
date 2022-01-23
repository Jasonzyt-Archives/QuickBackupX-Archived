#ifndef PCH_H
#define PCH_H
#define WIN32_LEAN_AND_MEAN
// Windows API
#include <windows.h>
// C++
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include <atomic>
#include <thread>
#include <queue>

#include <iostream>
#include <fstream>
#include <sstream>

#include <cstdio>
#include <ctime>

#undef max
#undef min
#undef ERROR
#if defined(BDS_LL)
#define PRINT lllog
#pragma comment(lib,"Lib/LiteLoader.lib")
#else
#define PRINT coutp.p
typedef std::string xuid_t;
#pragma comment(lib,"./LLSDK_1.16/LiteLoader.lib")
#endif

template <typename COMMITER>
class OLogger;
struct stdio_commit;
namespace fs = std::filesystem;
// Classes
class Backup;
// Variable
extern OLogger<stdio_commit*> coutp;

const std::string QBX_VERSION = "5.0.0-Beta";
const std::string QBX_FOLDER  = "./plugins/QuickBackupX/";
const std::string QBX_CONFIG  = QBX_FOLDER + "config.json";
const std::string QBX_RECORD  = QBX_FOLDER + "data.json";

#endif //PCH_H
