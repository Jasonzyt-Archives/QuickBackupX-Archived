#include "QuickBackupX.h"
#include "SyncCopy.h"
#include "Config.h"
#include "LangPack.h"

QuickBackupX::QuickBackupX() {
    cfg = std::make_unique<Config>();
    cfg->load(CONFIG.data());
    lpk = std::make_unique<LangPack>(cfg->language);
    lpk->load(LANGDIR.data());
    
}

void QuickBackupX::tick() {
    if (!copyQueue.empty()) {
        copyQueue.front().tick();
        if (copyQueue.front().done()) {
            copyQueue.pop();
        }
    }
}
