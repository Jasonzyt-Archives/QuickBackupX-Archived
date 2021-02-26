// Created by JasonZYT on 02/03/2021
#pragma once
#ifndef QBX_CONFIG_INCLUDED
#define QBX_CONFIG_INCLUDED
#include "pch.h"
#include "json/json.h"

namespace QuickBackupX
{
	class Config
	{
	public:
		Config();
		enum class PerType : int
		{
			Admin, Backup, Back
		};
		bool debug = false;
		bool acb = false; // Allow Command Block
		bool eula = false; // Agree EULA
		int lops = 10;
		std::string bop = ""; // Backup Output Path
		std::string los = "";
		std::string vos = "";
		std::vector<std::string> abtime;
		std::map<std::string, std::string> admins;
		std::map<std::string, std::string> backup; // Can Backup Player Vector
		std::map<std::string, std::string> back; // Can Back Player Vector
		bool getConfig();
		bool AddConfig();
		bool EditPermissionName(PerType per, std::string xuid, std::string name);
		bool PlayerIsAdmin(std::string name, std::string xuid);
		std::string getBackupDir();
		std::string getBackupDirC();
		std::string getBackupDisk();
	private:
		Json::Value cfgjv;
		bool getJsonArray(Json::Value root);
	};
}

#endif // QBX_JSON_INCLUDED
