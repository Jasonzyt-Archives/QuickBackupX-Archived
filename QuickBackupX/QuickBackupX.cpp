#include "pch.h"
#include "QuickBackupX.h"
#include "SyncCopy.h"
#include "Config.h"
#include "LangPack.h"
#include "AutoBackup.h"

QuickBackupX::QuickBackupX() {
    cfg = std::make_unique<Config>();
    cfg->load(CONFIG.data());
    lpk = std::make_unique<LangPack>(cfg->language);
    lpk->load(LANGDIR.data());
    ab = std::make_unique<AutoBackup>();
}

void QuickBackupX::tick() {
    if (!copyQueue.empty()) {
        copyQueue.front().tick();
        if (copyQueue.front().done()) {
            copyQueue.pop();
        }
    }
    ab->tick();
}
