#pragma once
#include "pch.h"
#include <nlohmann/json.hpp>

class Config {

public:

    bool allowCommandBlock = true;
    bool enableRemoteBackup = false;
    std::string backupOutputPath = "./backups";
    std::string remoteBackupServer = "http://address/path/";
    std::vector<std::string> autoBackupTime = {"08:00", "16:00", "00:00"};
    std::string language = "en_US";

    inline void save(const std::string& cfg) {
        nlohmann::json j{
            {"allowCommandBlock", allowCommandBlock},
            {"backupOutputPath", backupOutputPath},
            {"remoteBackup", {
                {"enabled", enableRemoteBackup},
                {"server", remoteBackupServer}
            }},
            {"autoBackupTime", autoBackupTime},
            {"language", language}
        };
        std::fstream file(cfg, std::ios::out | std::ios::trunc);
        file << std::setw(4) << j;
        file.close();
    }

    inline void load(const std::string& cfg) {
        if (!fs::exists(cfg)) {
            save(cfg);
        }
        nlohmann::json j;
        std::fstream file(cfg, std::ios::in);
        std::ostringstream oss;
        oss << file.rdbuf();
        file.close();
        try {
            j = nlohmann::json::parse(oss.str());
            if (j.count("remoteBackup")) {
                auto& setting = j["remoteBackup"];
                enableRemoteBackup = setting.value("enabled", false);
                remoteBackupServer = setting.value("server", "");
            }
            allowCommandBlock = j.value("allowCommandBlock", true);
            backupOutputPath = j.value("backupOutputPath", "./backups");
            autoBackupTime = j.value("autoBackupTime", std::vector<std::string>{});
            language = j.value("language", "en_US");
            logger.info(tr("Loaded config file: {}", cfg));
        }
        catch (nlohmann::detail::exception e) {
            if (e.id != 101) {
                logger.error("Error when parsing JSON: ", e.what());
            }
            logger.info("Try rewriting config file...");
            save(cfg);
        }
    }

};