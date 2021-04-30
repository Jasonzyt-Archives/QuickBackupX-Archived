// Created by JasonZYT on 2021/02/07
#pragma once
#ifndef QBX_BACKUP_INCLUDED
#define QBX_BACKUP_INCLUDED
#include "pch.h"
#include "func.h"
#include "file.h"
#include "json/json.h"
#include "qbzip.h"

#define Console_Type QuickBackupX::Backup::Executor::Type::Console
#define Player_Type  QuickBackupX::Backup::Executor::Type::Player
#define Block_Type   QuickBackupX::Backup::Executor::Type::Block
#define AutoBak_Type QuickBackupX::Backup::Executor::Type::AutoBackup
#define Unknown_Type QuickBackupX::Backup::Executor::Type::Unknown

namespace QuickBackupX
{
	class Backup
	{
	public:
		class Executor
		{
		public:
			enum class Type : int
			{
				All, Unknown, Console, Player, Block, AutoBackup
			};
			Type type = Type::Unknown;
			std::string pname = "";
			std::string pxuid = "";
			std::string cbpos = "";
			std::string to_string();
			std::string type_string();
			std::string format_pos();
			bool delsrcbak = false;
			bool delsrclevel = false;
		};

		Backup(std::string bnote = "");
		~Backup();
		bool Make  (Executor exer);
		bool Delete(Executor exer);
		bool Resume(Executor exer);

		std::map<std::string,std::string> flist;
		std::filesystem::path lpath;
		std::filesystem::path path;
		std::string md5 = "";
		std::string time = "";
		std::string note = "";
		Executor exer;
		size_t size = -1;
		int id = -1;
		void* outp;
	private:
		bool CheckBackupPermission();
		bool CheckDeletePermission(Executor exer);
		bool CheckResumePermission(Executor exer);
		bool Create();
	};
}

#endif // !QBX_BACKUP_INCLUDED
