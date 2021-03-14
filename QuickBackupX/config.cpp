// Created by JasonZYT on 02/03/2021
#include "pch.h"
#include "config.h"
#include "func.h"
#include "logger.h"

using namespace std;

namespace QuickBackupX
{
	Config::Config()
	{
		//this->getConfig();
	}

	bool Config::EditPermissionName(PerType per, string xuid, string name)
	{
		int i = 0;
		Json::Value jarr;
		switch (per)
		{
		case Config::PerType::Admin:
			jarr = this->cfgjv["Admin_Player"];
			break;
		case Config::PerType::Backup:
			jarr = this->cfgjv["Backup_Player"];
			break;
		case Config::PerType::Back:
			jarr = this->cfgjv["Back_Player"];
			break;
		default:
			return false;
			break;
		}
		int jarrs = jarr.size();
		while (i <= jarrs - 1)
		{
			if (jarr[i].empty()) continue;
			if (jarr[i]["Xuid"].asString() == xuid)
			{
				this->cfgjv[i]["Name"] = name;
				SWriteIntoFile(this->cfgjv, CONFIGFILE);
				return true;
			}
			i++;
		}
	}

	string Config::getBackupDir()
	{
		string dir;
		size_t pos = this->bop.find_last_of("/");
		if (pos == string::npos) pos = this->bop.find_last_of("\\");
		if (pos == string::npos) dir = "./";
		else dir = this->bop.substr(0, pos + 1);
		return dir;
	}
	string Config::getBackupDirC()
	{
		return filesystem::canonical(filesystem::path(this->getBackupDir())).string();
	}
	string Config::getBackupDisk()
	{
		string bdir = this->getBackupDirC();
		return bdir.substr(0, 3);
	}

	template<typename retvec>
	vector<retvec> Config::getConfigArray(string key)
	{
		int i = 0;
		vector<retvec> rv;
		Json::Value jv = this->cfgjv[key];
		int jvi = jv.size();
		if (jvi != 0)
		{
			while (i <= jvi - 1)
			{
				if (jv[i].empty()) continue;
				if (typeid(retvec) == typeid(string)) rv.push_back(jv[i].asString());
				//else if (typeid(retvec) == typeid(int)) rv.push_back(jv[i].asInt());
				i++;
			}
		}
		return rv;
	}
	map<string, string> Config::getPermission(Config::PerType per)
	{
		int i = 0;
		Json::Value jv;
		map<string, string> rm;
		switch (per)
		{
		case QuickBackupX::Config::PerType::Admin:
			jv = this->cfgjv["Admin_Player"];
			break;
		case QuickBackupX::Config::PerType::Backup:
			jv = this->cfgjv["Backup_Player"];
			break;
		case QuickBackupX::Config::PerType::Back:
			jv = this->cfgjv["Back_Player"];
			break;
		default:
			return rm;
			break;
		}
		int jvi = jv.size();
		if (jvi != 0)
		{
			i = 0;
			while (i <= jvi - 1)
			{
				if (jv[i].empty()) continue;
				if (!jv[i].isObject()) continue;
				this->admins.insert(
					pair<string, string>(
						jv[i]["Name"].asString(),
						jv[i]["Xuid"].asString()
					)
				);
				i++;
			}
		}
		return rm;
	}

	bool Config::getConfig()
	{
		this->cfgjv = getJSON(CONFIGFILE);
		Json::Value root = this->cfgjv;
		Json::Value example;
		example[0]["Name"] = "Steve";
		example[1]["Name"] = "Alex";
		example[1]["Xuid"] = "1145141919810";
		if (!root.isMember("Debug")            || !root["Debug"]           .isBool()  ) root["Debug"] = true;
		if (!root.isMember("Allow_CB")         || !root["Debug"]           .isBool()  ) root["Allow_CB"] = false;
		if (!root.isMember("BackupOutputPath") || !root["BackupOutputPath"].isString()) root["BackupOutputPath"] = "./backup/%Y-%m-%d-%H-%M.zip";
		if (!root.isMember("EULA")             || !root["EULA"]            .isBool()  ) root["EULA"] = false;
		if (!root.isMember("ListOutputSize")   || !root["ListOutputSize"]  .isInt()   ) root["ListOutputSize"] = 10;
		if (!root.isMember("ListOutputCont")   || !root["ListOutputCont"]  .isString()) root["ListOutputCont"] = u8"备份[%onum%] %date% %time% %size%";
		if (!root.isMember("ViewOutputCont")   || !root["ViewOutputCont"]  .isString()) root["ViewOutputCont"] = u8"备份[%onum%] %date% %time% %size% %exer%";
		if (!root.isMember("Admin_Player")     || !root["Admin_Player"]    .isArray() ) root["Admin_Player"] = example;
		if (!root.isMember("Backup_Player")    || !root["Backup_Player"]   .isArray() ) root["Backup_Player"] = example;
		if (!root.isMember("Back_Player")      || !root["Back_Player"]     .isArray() ) root["Back_Player"] = example;
		if (!root.isMember("AutoBackup_Time")  || !root["AutoBackup_Time"] .isArray() ) root["AutoBackup_Time"][0] = "Hour:Minute";
		if (!root.isMember("Auto_On_AB")       || !root["Auto_On_AB"]      .isBool()  ) root["Auto_On_AB"] = false;
		if (!root.isMember("Compression")      || !root["Compression"]     .isInt()   ) root["Compression"] = 60;
		if (!root.isMember("AB_Max")           || !root["AB_Max"]          .isInt()   ) root["AB_Max"] = 10;
		SWriteIntoFile(root, CONFIGFILE);
		if (!root["EULA"].asBool())
		{
			PRERR(u8"您未同意EULA,请在" << CONFIGFILE << u8"中将\"EULA\": false改为\"EULA\": true,进程即将结束");
			Sleep(3000);
			exit(-1);
		}
		this->debug = root["Debug"].asBool();
		this->acb = root["Allow_CB"].asBool();
		this->bop = root["BackupOutputPath"].asString();
		this->eula = root["EULA"].asBool();
		this->lops = root["ListOutputSize"].asInt();
		this->los = root["ListOutputCont"].asString();
		this->vos = root["ViewOutputCont"].asString();
		this->aoab = root["Auto_On_AB"].asBool();
		this->abm = root["AB_Max"].asInt();
		this->csl = root["Compression"].asInt();
		this->abtime = this->getConfigArray<string>("AutoBackup_Time");
		this->admins = this->getPermission(Config::PerType::Admin);
		this->backup = this->getPermission(Config::PerType::Backup);
		this->back = this->getPermission(Config::PerType::Back);
		if (this->csl < 0) this->csl = 0;
		else if (this->csl > 100) this->csl = 100;
		if (this->abm <= 0) this->abm = 0;
		Debug{
			L_INFO("正在读取配置: ");
			L_INFO(string("- Debug: ") + (this->debug ? "true" : "false"));
			L_INFO(string("- EULA: ") + (this->eula ? "true" : "false"));
			L_INFO(string("- AB_Max: " + to_string(this->abm)));
			L_INFO(string("- Allow_CB: ") + (this->acb ? "true" : "false"));
			L_INFO(string("- Auto_On_AB: ") + (this->aoab ? "true" : "false"));
			L_INFO(string("- BackupOutputPath: ") + this->bop);
			L_INFO(string("- ListOutputCont: ") + this->los);
			L_INFO(string("- ViewOutputCont: ") + this->vos);
			L_INFO(string("- ListOutputSize: ") + to_string(this->lops));
			L_INFO(string("- Compression: " + to_string(this->csl)));
			L_INFO(string("- Admin_Player(") + to_string(this->admins.size()) + ") ");// + this->cfgjv["Admin_Player"].asString());
			L_INFO(string("- Backup_Player(") + to_string(this->backup.size()) + ") ");// + this->cfgjv["Backup_Player"].asString());
			L_INFO(string("- Back_Player(") + to_string(this->back.size()) + ") ");// +this->cfgjv["Back_Player"].asString());
			L_INFO(string("- AutoBackup_Time(") + to_string(this->abtime.size()) + ") ");
		}
		L_INFO("配置读取成功!");
		PR(u8"配置读取成功!");
		return true;
	}
}