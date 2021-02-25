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
		std::string bop = ""; // Backup Output Path
		std::map<std::string, std::string> admins;
		//std::map<std::string, std::string> backupc; // Can Backup to Cloud Player Vector
		std::map<std::string, std::string> backup; // Can Backup Player Vector
		//std::map<std::string, std::string> backc; // Can Back form Cloud Player Vector
		std::map<std::string, std::string> back; // Can Back Player Vector
		bool getConfig();
		bool AddConfig();
		bool EditPermissionName(PerType per, std::string xuid, std::string name);
		bool PlayerIsAdmin(std::string name, std::string xuid);
		std::string getBackupDir();
		std::string getBackupDisk();
	private:
		Json::Value cfgjv;
		bool getJsonArray(Json::Value root);
	};

	const std::string cfgexample = R"({"Allow_CB":false,"Debug":true,"BackupOutputPath":"./backup/%Y-%m-%d-%H-%M.zip","EULA":false,"Backup_Player":[{"Name":"Steve","Xuid":"1145141919810"},{"Name":"Alex"}],"Back_Player":[],"Admin_Player":[]})";
}

#endif // QBX_JSON_INCLUDED
