// Created by Jasonzyt on 2021/02/26
#pragma once
#ifndef QBX_AUTOBACKUP_INCLUDED
#define QBX_AUTOBACKUP_INCLUDED
#include "pch.h"
#include "config.h"
#include "backup.h"
#include "json.h"
#include "logger.h"

namespace QuickBackupX
{
	class AutoBackup
	{
	public:
		bool Run();
		bool Init();
		bool is_on = false; // 若要关闭自动备份,直接将此变量设为false即可
		std::vector<std::time_t> time;
	//private:
		int getLocalMinute();
		int getLocalHour();
	};
}

#endif // !QBX_AUTOBACKUP_INCLUDED
