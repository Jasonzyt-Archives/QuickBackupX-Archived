#include "pch.h"
#include "AutoBackup.h"
#include "LangPack.h"
#include "Config.h"
#include "QuickBackupX.h"
#include "Logger.h"
#if defined(BDS_LL)
#include <HookAPI.h>
#else 
#include <loader/Loader.h>
#endif

using namespace std;

Logger logger("QBX");
std::unique_ptr<QuickBackupX> plugin;

void entry() {
    logger.info("QuickBackupX v{} loaded! Author: Jasonzyt [{}]", 
                QuickBackupX::VERSION, __DATE__ " " __TIME__);
}

THook(void, "?tick@ServerLevel@@UEAAXXZ", void* thiz) {
    plugin->tick();
    return original(thiz);
}
