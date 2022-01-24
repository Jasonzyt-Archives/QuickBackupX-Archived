#pragma once
#include "pch.h"
#include "Utils.h"
#include <nlohmann/json.hpp>

class LangPack {

	std::string langType;
	std::unordered_map<std::string, std::unordered_map<std::string, std::string>> lang;
	nlohmann::json defaultLang{
		{"en_US", {
			}
		},
		{"zh_CN", {
			}
		}
	};

	inline void writeDefault(const std::string& fn) {
		std::fstream file(fn, std::ios::out | std::ios::ate);
		file << std::setw(4) << defaultLang;
		file.close();
	}

	inline void init(const std::string& fn) {
		nlohmann::json json;
		auto lastSlash = fn.find_last_of('/');
		auto lastBackslash = fn.find_last_of('\\');
		fs::create_directories(fn.substr(0, std::max((lastSlash != std::string::npos ? lastSlash : 0),
			(lastBackslash != std::string::npos ? lastBackslash : 0))));
		std::fstream file(fn, std::ios::in | std::ios::app);
		std::ostringstream oss;
		bool flag = false;
		oss << file.rdbuf();
		file.close();
		if (oss.str().empty()) {
			writeDefault(fn);
		}

		try {
			json = nlohmann::json::parse(oss.str());
		}
		catch (nlohmann::detail::exception e) {
			if (e.id != 101) {
				PRINT<ERROR, RED>("Error when parse JSON: ", e.what());
			}
			PRINT<ERROR, RED>("Try rewriting langpack file...");
			writeDefault(fn);
			return;
		}
		json.get_to(lang);
	}

public:

	LangPack(const std::string& file, const std::string& language) {
		langType = language;
		init(file);
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
		if (defaultLang["en_US"].count(key)) {
			return defaultLang["en_US"][key].get<std::string>();
		}
		PRINT<WARN, YELLOW>("Could not find the translation for ", key);
		return key;
	}
	inline std::string get(const std::string& key) {
		return get(langType, key);
	}
	template <typename ... Args>
	inline std::string localize(const std::string& key, Args... args) {
		return format(get(key).c_str(), args...);
	}

};
