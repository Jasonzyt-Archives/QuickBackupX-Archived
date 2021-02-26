// Created by Jasonzyt on 2021/02/26
#pragma once
#ifndef QBX_AUTOBACKUP_INCLUDED
#define QBX_AUTOBACKUP_INCLUDED
#include "pch.h"
#include "config.h"
#include "backup.h"
#include "logger.h"

namespace QuickBackupX
{
	class AutoBackup
	{
	public:
		AutoBackup();
		bool Run();
		std::vector<std::vector<int>> time;
	private:
		int getLocalMinute();
		int getLocalHour();
	};
}

#endif // !QBX_AUTOBACKUP_INCLUDED
