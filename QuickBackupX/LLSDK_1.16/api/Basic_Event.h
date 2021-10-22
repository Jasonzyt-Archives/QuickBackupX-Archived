#pragma once
#include <string>
#include <liteloader.h>
#include <functional>
#include <mc\mass.h>
using namespace std;
typedef unsigned long long xuid_t;

class CommandRegistry;
class RegCmdEV {
public:
	CommandRegistry* CMDRg;
};

namespace Event {
	LIAPI void addEventListener(function<void(RegCmdEV)> callback);
};