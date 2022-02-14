#ifndef PCH_H
#define PCH_H
#define WIN32_LEAN_AND_MEAN
// Windows API
#include <Windows.h>
// C++
#include <string>
#include <string_view>
#include <map>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <thread>
#include <queue>

#include <iostream>
#include <fstream>
#include <sstream>

#include <cstdio>
#include <ctime>

#include "Logger.h"
#undef max
#undef min
#undef ERROR
#if defined LLSDK
#pragma comment(lib,"Lib/LiteLoader.lib")
#else
typedef std::string xuid_t;
#pragma comment(lib,"./LLSDK_1.16/LiteLoader.lib")
#endif

namespace fs = std::filesystem;
// Classes
class QuickBackupX;
// Variables
extern Logger logger;
extern std::unique_ptr<QuickBackupX> plugin;
// Functions
template <typename ... Args>
inline std::string tr(const std::string& k, Args... args);

#endif //PCH_H
