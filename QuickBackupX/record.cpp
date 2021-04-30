// Created by JasonZYT on 2021/02/09
#include "record.h"

using namespace std;

namespace QuickBackupX
{
	BRecord::BRecord()
	{
		sql.Table_Create("BRecord", 
		 R"("ID" UNSIGNED INTEGER NOT NULL, "Time" DATETIME NOT NULL, "Path" TEXT NOT NULL, "MD5" CHAR(32) NOT NULL, "Size" UNSIGNED BIG INT, "Executor" CHAR(256) NOT NULL, "Note" CHAR(256) NOT NULL DEFAULT '-')"
		);
	}

	bool BRecord::AddRecord(Backup* bak)
	{
		this->blist.push_back(bak);
		Debug{ L_INFO(string("写入记录: ") + BACKUPRECFILE + " ..."); }
		ostringstream insert;
		string exerinfo;
		switch (bak->exer.type)
		{
		case Unknown_Type:
			exerinfo = "1";
			break;
		case Console_Type:
			exerinfo = "2";
			break;
		case Player_Type:
			exerinfo = "3;" + bak->exer.pname + ";" + bak->exer.pxuid;
			break;
		case Block_Type:
			exerinfo = "4;" + bak->exer.cbpos;
			break;
		case AutoBak_Type:
			exerinfo = "5";
			break;
		}
		insert << bak->id << ",\"" << bak->time << "\","
			<< bak->path << ",\"" << bak->md5 << "\","
			<< bak->size << ",\"" << exerinfo << "\"" << (bak->note.empty() ? "" : ",\"" + bak->note + "\"");
		bool res;
		if (bak->note.empty()) res = sql.Insert("BRecord", "`ID`,`Time`,`Path`,`MD5`,`Size`,`Executor`", insert.str());
		else res = sql.Insert("BRecord", insert.str());
		res ? L_INFO("记录写入成功!") : L_ERROR("记录写入失败!!!");
		return res;
	}

	vector<Backup*> BRecord::ListRecord()
	{
		Debug{ L_INFO("遍历备份记录..."); }
		vector<Backup*> bak;
		SQLite::BRecResult* bres = sql.Select_All("BRecord");
		int rs = bres->res_size;
		for (int i = 0; i < rs; i++)
		{
			Backup* temp = new Backup;
			int bid_i = bres->id[i];
			Debug{ L_INFO(string("- 正在遍历备份[") + to_string(bid_i) + "]"); }
			temp->id = bid_i;
			temp->time = bres->time[i];
			temp->path = filesystem::path(bres->path[i]);
			temp->exer = bres->exer[i];
			temp->md5 = bres->md5[i];
			temp->size = bres->size[i];
			temp->note = bres->note[i];
			if (!filesystem::exists(bres->path[i]))
			{
				Debug{ L_WARNING(string("- 未找到备份[") + to_string(bid_i) + "]文件,删除记录..."); }
				bool res = sql.Delete_Row("BRecord", "ID = " + to_string(bid_i));
				for (int j = bid_i + 1; j <= bres->res_size; j++)
				{
					sql.Update("BRecord", "ID = " + to_string(j - 1), "ID = " + to_string(j));
				}
				continue;
			}
			bak.push_back(temp);
		}
		delete bres;
		return bak;
	}

	bool BRecord::RefrashRecord()
	{
		for (auto& it : this->blist)
		{
			if (!filesystem::exists(it->path)) this->DeleteRecord(it->id);
		}
		return true;
	}

	Backup* QuickBackupX::BRecord::GetOldestRecord(Backup::Executor::Type tp)
	{
		for (auto& it : this->blist)
		{
			if (!filesystem::exists(it->path)) this->DeleteRecord(it->id);
			else
			{
				if (it->exer.type == tp) return it;
			}
		}
		return NULL;
	}

	bool BRecord::DeleteRecord(int id)
	{
		this->blist.erase(blist.begin() + (id - 1));
		bool res = sql.Delete_Row("BRecord", "ID = " + to_string(id));
		for (auto& it : blist)
		{
			if (it->id < id) continue;
			sql.Update("BRecord", "ID = " + to_string((it->id) - 1), "ID = " + to_string(it->id--));
		}
		return res;
	}

	int BRecord::GetTotal(Backup::Executor::Type tp)
	{
		if (tp == Backup::Executor::Type::All) return this->blist.size();
		int i = 0;
		for (auto& it : this->blist)
		{
			if (it->exer.type == tp) i++;
		}
		return i;
	}
}
