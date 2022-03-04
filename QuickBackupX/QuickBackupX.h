#pragma once
#include "SyncCopy.h"
#define PLUGIN_DIR "./plugins/QuickBackupX/"

class Config;
class LangPack;
class SyncCopy;
class Backup;
class AutoBackup;

class QuickBackupX {

public:

    std::unique_ptr<Config> cfg;
    std::unique_ptr<LangPack> lpk;
    std::queue<SyncCopy> copyQueue;
    std::unique_ptr<AutoBackup> ab;

    QuickBackupX();
    void tick();
    void reloadBackups();

    static constexpr std::string_view VERSION = "5.0.0-Beta";
    static constexpr std::string_view CONFIG  = PLUGIN_DIR "config.json";
    static constexpr std::string_view LANGDIR = PLUGIN_DIR "lang/";
    static constexpr std::string_view RECORD  = PLUGIN_DIR "backups.json";

};