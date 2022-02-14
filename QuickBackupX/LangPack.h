#pragma once
#include "pch.h"
#include "QuickBackupX.h"
#include "Utils.h"
#include <nlohmann/json.hpp>

class LangPack {

    std::string consoleLang;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> lang;
    std::unordered_map<std::string, nlohmann::json> defaultLang{
        {"zh_CN", {
                {"Wrong AutoBackup time format: {}. Ignored.", "错误的自动备份时间格式: {}, 已忽略"},
                {"Wrong time: {}. Ignored.", "错误的时间: {}, 已忽略"},
                {"Loaded config file: {}", "已加载配置文件: {}"}
            }
        }
    };

    inline void rewriteDefault(const std::string& k) {
        auto path = QuickBackupX::LANGDIR.data() + k + ".json";
        if (fs::exists(path)) {
            std::fstream file(path, std::ios::out | std::ios::ate);
            file << std::setw(4) << defaultLang[k];
            file.close();
        }
    }

    inline void writeDefault() {
        for (auto& [k, v] : defaultLang) {
            auto path = QuickBackupX::LANGDIR.data() + k + ".json";
            if (!fs::exists(path)) {
                std::fstream file(path, std::ios::out | std::ios::app);
                file << std::setw(4) << v;
                file.close();
            }
        }
    }

public:

    LangPack(const std::string& language) {
        consoleLang = language;
    }

    inline void load(const std::string& dir) {
        writeDefault();
        if (!fs::exists(dir)) fs::create_directories(dir);
        for (auto& p : fs::directory_iterator(dir)) {
            if (p.path().extension() != ".json") {
                continue;
            }
            auto langName = p.path().stem().string();
            nlohmann::json json;
            std::fstream file(p.path().string(), std::ios::in);
            std::ostringstream oss;
            oss << file.rdbuf();
            file.close();
            try {
                json = nlohmann::json::parse(oss.str());
                json.get_to(lang[langName]);
            }
            catch (nlohmann::detail::exception e) {
                if (e.id != 101) {
                    logger.error("Error when parsing JSON: ", e.what());
                }
                logger.info("Try rewriting langpack file...");
                rewriteDefault(langName);
                return;
            }
        }
    }

    inline std::string get(const std::string& lcode, const std::string& key) {
        if (lang.count(lcode)) {
            auto& l = lang.at(lcode);
            if (l.count(key)) {
                return l.at(key);
            }
            else {
                if (defaultLang.count(lcode) && defaultLang.at(lcode).count(key)) {
                    return defaultLang[lcode][key].get<std::string>();
                }
            }
        }
        return key;
    }
    inline std::string get(const std::string& key) {
        return get(consoleLang, key);
    }

    template <typename ... Args>
    inline std::string localize(const std::string& key, Args... args) {
        return fmt::format(get(key).c_str(), args...);
    }

};


template <typename ... Args>
inline std::string tr(const std::string& k, Args... args) {
    return plugin->lpk->localize(k, args...);
}
