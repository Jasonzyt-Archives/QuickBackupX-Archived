// Created by Jasonzyt on 2021/02/26
#include "autobackup.h"
#pragma warning(disable:4996)
#undef min

using namespace std;

namespace QuickBackupX
{
	AutoBackup::AutoBackup()
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
				vector<int> timevec;
				timevec.push_back(hour);
				timevec.push_back(min);
				this->time.push_back(timevec);
			}
		}
	}

	bool AutoBackup::Run()
	{
		vector<vector<int>>::iterator iter = this->time.begin();
		for (; iter != this->time.end(); iter++)
		{
			vector<int> vec = (*iter);
			if (vec[0] == this->getLocalHour())
			{
				if (vec[1] == this->getLocalMinute())
				{
					L_INFO("执行自动备份...");
					Backup* bak = new Backup;
					Backup::Executor exer;
					exer.type = Backup::Executor::Type::AutoBackup;
					thread runbak(&Backup::Make, bak, exer);
					runbak.detach();
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