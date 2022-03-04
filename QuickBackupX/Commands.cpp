#include "pch.h"
#include "QuickBackupX.h"
#include "Logger.h"
#if defined BDS_LL
#include "RegCommandAPI.h"
#else
#include <api/commands.h>
#endif

enum class Operation {
    Make,
    Reseum,
    Delete,
    List,
    Auto,
    Help
};

void onCmdMake(const CommandOrigin& ori, CommandOutput& outp) {
    
}