// Created by JasonZYT on 2021/02/09
#pragma once
#ifndef QBX_RECORD_INCLUDED
#define QBX_RECORD_INCLUDED
#include "pch.h"
#include "logger.h"
#include "config.h"
#include "backup.h"
#include "sqlite.h"

namespace QuickBackupX
{
	class BRecord
	{
	public:
		BRecord();
		bool AddRecord(Backup* bak);
		bool DeleteRecord(int id);
		bool RefrashRecord();
		Backup* GetOldestRecord(Backup::Executor::Type tp);
		int GetTotal(Backup::Executor::Type tp);
		std::vector<Backup*> ListRecord();

		SQLite sql = SQLite(BACKUPRECFILE);
		Json::Value root;
		std::vector<Backup*> blist;
	};
}

#endif // !QBX_JSON_INCLUDED

