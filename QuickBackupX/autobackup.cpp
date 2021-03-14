// Created by Jasonzyt on 2021/02/26
#include "autobackup.h"
#pragma warning(disable:4996)
#undef min

using namespace std;

namespace QuickBackupX
{
	bool AutoBackup::Init()
	{
		vector<string>::iterator iter = cfg->abtime.begin();
		for (; iter != cfg->abtime.end(); iter++)
		{
			regex rstr("([0-9]+):([0-9]+)");
			if (regex_match((*iter), rstr))
			{
				vector<string> ham = split((*iter), ':');
				int hour = atoi(string(ham[0]).c_str());
				int min = atoi(string(ham[1]).c_str());
				if (hour >= 24) continue;
				if (min >= 60) continue;
				tm time;
				time.tm_hour = hour;
				time.tm_min = min;
				this->time.push_back(mktime(&time));
			}
		}
		return true;
	}

	bool AutoBackup::Run()
	{
		if (this->is_on)
		{
			vector<time_t>::iterator iter = this->time.begin();
			for (; iter != this->time.end(); iter++)
			{
				tm time;
				time.tm_hour = this->getLocalHour();
				time.tm_min = this->getLocalMinute();
				time_t timt = (*iter);
				time_t now = mktime(&time);
				if (timt == now)
				{
					Backup::Executor exer;
					exer.type = Backup::Executor::Type::AutoBackup;
					L_INFO("执行自动备份...");
					if (rec->GetTotal(AutoBak_Type) >= cfg->abm)
					{
						rec->GetOldestRecord(AutoBak_Type)->Delete(exer);
					}
					Backup* bak = new Backup;
					thread runbak(&Backup::Make, bak, exer);
					runbak.detach();
					//delete bak;
					return true;
				}
			}
		}
		return false;
	}

	int AutoBackup::getLocalHour()
	{
		time_t timep;
		::time(&timep);
		char tmp[64];
		strftime(tmp, sizeof(tmp), "%H", localtime(&timep));
		return atoi(tmp);
	}

	int AutoBackup::getLocalMinute()
	{
		time_t timep;
		::time(&timep);
		char tmp[64];
		strftime(tmp, sizeof(tmp), "%M", localtime(&timep));
		return atoi(tmp);
	}
}