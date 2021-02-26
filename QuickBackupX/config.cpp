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

	bool Config::PlayerIsAdmin(string name, string xuid)
	{
		map<string, string>::iterator iter = cfg->admins.begin();
		for (; iter != cfg->backup.end(); ++iter)
		{
			if (iter->first == name)
			{
				if (iter->second != xuid)
				{
					PRWARN(u8"玩家 " << name << " Admin权限检查失败: Xuid不匹配");
					L_WARNING(string("玩家 ") + name + " Admin权限检查失败: Xuid不匹配");
					return false;
				}
				else return true;
			}
			if (iter->second == xuid)
			{
				if (iter->first != name)
					cfg->EditPermissionName(Config::PerType::Admin, xuid, name);
				return true;
			}
		}
		PRWARN(u8"玩家 " << name << " Admin权限检查失败: 无权限");
		L_WARNING(string("玩家 ") + name + " Admin权限检查失败: 无权限");
		return false;
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

	bool Config::getJsonArray(Json::Value root)
	{
		int i = 0;
		Json::Value abtv = root["AutoBackup_Time"];
		int abti = abtv.size();
		if (abti != 0)
		{
			i = 0;
			while (i <= abti - 1)
			{
				if (abtv[i].empty()) continue;
				this->abtime.push_back(abtv[i].asString());
				i++;
			}
		}
		Json::Value adminv = root["Admin_Player"];
		int admini = adminv.size();
		if (admini != 0)
		{
			i = 0;
			while (i <= adminv.size() - 1)
			{
				if (adminv[i].empty()) continue;
				this->admins.insert(
					pair<string, string>(
						adminv[i]["Name"].asString(),
						adminv[i]["Xuid"].asString()
					)
				);
				i++;
			}
		}
		Json::Value backup = root["Backup_Player"];
		Json::Value back = root["Back_Player"];
		int backupi = backup.size();
		int backi = back.size();
		if (backupi != 0)
		{
			i = 0;
			while (i <= backup.size() - 1)
			{
				if (backup[i].empty()) continue;
				this->backup.insert(
					pair<string, string>(
						backup[i]["Name"].asString(),
						backup[i]["Xuid"].asString()
					)
				);
				i++;
			}
		}
		if (backi != 0)
		{
			i = 0;
			while (i <= back.size() - 1)
			{
				if (back[i].empty()) continue;
				this->back.insert(
					pair<string, string>(
						back[i]["Name"].asString(),
						back[i]["Xuid"].asString()
					)
				);
				i++;
			}
		}
		return true;
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
		this->getJsonArray(root);
		Debug{
			L_INFO("正在读取配置: ");
			L_INFO(string("- Debug: ") + (this->debug ? "true" : "false"));
			L_INFO(string("- Allow_CB: ") + (this->acb ? "true" : "false"));
			L_INFO(string("- EULA: ") + (this->eula ? "true" : "false"));
			L_INFO(string("- BackupOutputPath: ") + this->bop);
			L_INFO(string("- ListOutputCont: ") + this->los);
			L_INFO(string("- ViewOutputCont: ") + this->vos);
			L_INFO(string("- ListOutputSize: ") + to_string(this->lops));
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