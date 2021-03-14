// Created by JasonZYT on 2021/02/09
#pragma once
#ifndef QBX_JSON_INCLUDED
#define QBX_JSON_INCLUDED
#include "pch.h"
#include "logger.h"
#include "config.h"
#include "backup.h"

namespace QuickBackupX
{
	class BRecJson
	{
	public:
		BRecJson();
		bool AddRecord(Backup* bak);
		bool DeleteRecord(int num);
		bool RefrashRecord();
		Backup* GetOldestRecord(Backup::Executor::Type tp);
		int GetTotal(Backup::Executor::Type tp);
		std::vector<Backup*> ListRecord();

		Json::Value root;
		std::vector<Backup*> blist;
	};
}

#endif // !QBX_JSON_INCLUDED

