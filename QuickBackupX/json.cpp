// Created by JasonZYT on 2021/02/09
#include "json.h"

using namespace std;

namespace QuickBackupX
{
	BRecJson::BRecJson()
	{
		//Debug{ L_INFO("备份记录 BRecJson 对象已构造"); }
		this->root = getJSON(BACKUPRECFILE);
		//this->blist = ListRecord();
	}

	bool BRecJson::AddRecord(Backup* bak)
	{
		this->blist.push_back(bak);
		Debug{ L_INFO(string("写入记录文件: ") + BACKUPRECFILE + " ..."); }
		Json::Value in;
		in["Path"] = bak->path.string();
		in["MD5"] = bak->md5;
		in["Executor"]["Type"] = (int)bak->exer.type;
		switch (bak->exer.type)
		{
		case Unknown_Type:
			break;
		case Console_Type:
			break;
		case Player_Type:
			in["Executor"]["Name"] = bak->exer.pname;
			in["Executor"]["Xuid"] = bak->exer.pxuid;
			break;
		case Block_Type:
			in["Executor"]["Pos"] = bak->exer.cbpos;
			in["Executor"]["Mode"] = (int)bak->exer.cbmode;
			break;
		}
		in["Size"] = bak->size;
		in["Time"] = bak->time;
		this->root.append(in);
		bool res = SWriteIntoFile(this->root, BACKUPRECFILE);
		res ? L_INFO("记录写入成功!") : log->LogError("记录写入失败!!!");
		return res;
	}

	int BRecJson::getBackupQuantity()
	{
		return this->root.size();
	}

	vector<Backup*> BRecJson::ListRecord()
	{
		Debug{ L_INFO("遍历备份记录..."); }
		int rs = this->root.size();
		vector<Backup*> bak;
		for (int i = 0; i <= rs - 1; i++)
		{
			Debug{ L_INFO(string("- 正在遍历备份[") + to_string((i + 1)) + "] " + this->root[i]["Time"].asString()); }
			if (!filesystem::exists(this->root[i]["Path"].asString()))
			{
				Debug{ L_WARNING(string("- 未找到备份[") + to_string((i + 1)) + "]文件,删除记录..."); }
				Json::Value jv1;
				bool jdres = this->root.removeIndex((i - 1), &jv1);
				SWriteIntoFile(this->root, BACKUPRECFILE);
				continue;
			}
			Backup* temp = new Backup;
			Backup::Executor exert;
			exert.type = (Backup::Executor::Type)(this->root[i]["Executor"]["Type"].asInt());
			switch (exert.type)
			{
			case Unknown_Type: break;
			case Console_Type: break;
			case Player_Type:
				exert.pname = this->root[i]["Executor"]["Name"].asString();
				exert.pxuid = this->root[i]["Executor"]["Xuid"].asString();
				break;
			case Block_Type:
				exert.cbpos = this->root[i]["Executor"]["Pos"].asString();
				exert.cbmode = (Backup::Executor::CBMode)(this->root[i]["Executor"]["Mode"].asInt)();
				break;
			}
			temp->onum = i + 1;
			temp->time = this->root[i]["Time"].asString();
			temp->path = filesystem::path(this->root[i]["Path"].asString());
			temp->exer = exert;
			temp->md5 = this->root[i]["MD5"].asString();
			temp->size = this->root[i]["Size"].asUInt64();
			bak.push_back(temp);
		}
		return bak;
	}

	bool BRecJson::DeleteRecord(int num)
	{
		vector<Backup*>::iterator it = this->blist.begin() + (num - 1);
		this->blist.erase(it);
		for (; it != this->blist.end(); ++it)
		{
			(*it)->onum--;
		}
		//bool jdres = this->root.isremove((num - 1));
		Json::Value jv1;
		bool jdres = this->root.removeIndex((num - 1), &jv1);
		SWriteIntoFile(this->root, BACKUPRECFILE);
		return jdres;
	}
}