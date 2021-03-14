// Created by JasonZYT on 2021/02/07
#include "backup.h"
#include "config.h"
#include "logger.h"
#include "json.h"
#include "md5.h"
#pragma warning(disable:4996)

#define to_GB(x) ((x.HighPart << 2) + (x.LowPart >> 20) / 1024.0)
#define to_Byte(x) (size_t)((x.HighPart << 2) + ((DWORD)x.LowPart))

using namespace std;

namespace QuickBackupX
{
	string Backup::Executor::to_string()
	{
		if (type == Console_Type)     return "[Console]";
		else if (type == Player_Type) return string("[Player]") + pname;
		else if (type == Block_Type)  return string("[CMDBlock]") + cbpos;
		else if (type == Backup::Executor::Type::AutoBackup) return "[AutoBackup]";
		return "[Unknown]";
	}

	string Backup::Executor::type_string()
	{
		switch (type)
		{
		case Unknown_Type:
			return "Unknown";
			break;
		case Console_Type:
			return "Console";
			break;
		case Player_Type:
			return "Player";
			break;
		case Block_Type:
			return "Block";
			break;
		default:
			return "AutoBackup";
			break;
		}
	}

	string Backup::Executor::format_pos()
	{
		string rv;
		if (type != Block_Type)
		{
			L_ERROR("不是Block类型!!!");
			throw 104;
		}
		if (cbpos == "")
		{
			L_ERROR("cbpos为空!!!");
			throw 105;
		}
		vector<string> pos = split(cbpos, ':');
		if (pos.size() != 2) throw 106;
		string dim = "未知维度";
		switch (atoi(pos[0].c_str()))
		{
		case 0: dim = "主世界"; break;
		case 1: dim = "下界"; break;
		case 2: dim = "末地"; break;
		}
		rv = dim + ":" + pos[1];
		return rv;
	}

	Backup::Backup()
	{
		string lp = "./worlds/" + getLevelName() + "/";
		this->lpath = filesystem::path(lp);
		this->onum = rec->blist.size() + 1;
	}

	bool Backup::Make(Backup::Executor exer)
	{
		this->exer = exer;
		this->time = getTime();
		Directory leveld(this->lpath);
		string bdisk = cfg->getBackupDisk();
		size_t lsize = leveld.dirsize();
		if (!this->CheckBackupPermission())
		{
			if (this->exer.type == Player_Type)
			{
				PRWARN(u8"玩家 " << this->exer.pname << u8" 试图执行备份,但因无权限被拦截了");
				L_WARNING(string("玩家 ") + this->exer.pname + "试图执行备份(无权限)");
				TellAdmin(string("玩家 ") + this->exer.pname + "(无权限) 试图执行备份");
				sendText(this->exer.pname, "§c您没有权限执行此操作!!!");
			}
			else if (this->exer.type == Block_Type)
			{
				PRWARN(u8"命令方块(位于 " << this->exer.cbpos << u8") 试图执行备份,但因配置文件不允许被拦截了");
				L_WARNING(string("命令方块(位于 ") + this->exer.cbpos + ") 试图执行备份");
			}
			return false;
		}
		PR(u8"创建备份...");
		runcmd("save hold");
		this_thread::sleep_for(chrono::milliseconds(2500));
		sendText("all", string("§b[QuickBackupX] 开始创建备份: ") + this->time);
		ULARGE_INTEGER bavfree;
		ULARGE_INTEGER btotal;
		ULARGE_INTEGER bfree;
		GetDiskFreeSpaceExA(bdisk.c_str(), &bavfree, &btotal, &bfree);
		if (to_Byte(bavfree) <= lsize)
		{
			PRWARN(u8"磁盘可用空间少于当前存档大小,已取消备份!");
			sendText("all", "§c磁盘可用空间少于当前存档大小,已取消备份!");
			return false;
		}
		LARGE_INTEGER freq;
		LARGE_INTEGER begin;
		LARGE_INTEGER end;
		double timec;
		Debug{ L_INFO("备份开始计时"); }
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&begin);
		ZRESULT res = this->Create(); // 执行备份
		QueryPerformanceCounter(&end);
		timec = (double)(end.QuadPart - begin.QuadPart) / (double)freq.QuadPart; // 获取计时结果

		if (res == ZR_OK)
		{
			Debug{ L_INFO("开始获取备份信息"); }
			if (!filesystem::exists(this->path))
			{
				PRERR(u8"备份文件不存在");
				PRERR(u8"备份失败!!!");
				sendText("all", "§c备份失败!");
				return false;
			}
			this->size = getFileSize(this->path.string());
			FILE* fp = fopen(this->path.string().c_str(), "rb");
			this->md5 = md5file(fp);
			fclose(fp);
			rec->AddRecord(this);
			int quan = rec->blist.size();
			string ssize = SizeToString(this->size);
			string disk = this->path.string().substr(0, 3);

			ULARGE_INTEGER avfree;
			ULARGE_INTEGER total;
			ULARGE_INTEGER free;
			GetDiskFreeSpaceExA(disk.c_str(), &avfree, &total, &free);
			if (to_GB(total) == 0)
			{
				L_ERROR("除零警告!!!");
				PRERR(u8"除零警告!!!");
				Sleep(3000);
				throw 103;
				return false;
			}
			float diskur = (to_GB(total) - to_GB(avfree)) / to_GB(total) * 100;
			float diskus = to_GB(total) - to_GB(free);
			double fcompr = ((double)this->size / lsize) * 100;
			string compr = round_str(fcompr, 2) + "%";

			L_INFO("备份成功(0)");
			PR(u8"备份完成! (耗时 " << timec << u8" 秒)");
			sendText("all", string("§a备份完成! (耗时 ") + to_string(timec) + u8" 秒)");
			PR(u8"- 此次备份存档大小: " << ssize << "(" << this->size << " B)");
			PR(u8"- 此次备份存档路径: " << this->path.string());
			PR(u8"- 此次备份存档MD5: " << this->md5);
			PR(u8"- 此次备份的执行者: " << this->exer.to_string());
			PR(u8"- 此次备份的压缩率: " << compr << u8" (原存档大小:" << SizeToString(lsize) << ")");
			PR(u8"- 备份后的" << disk[0] << u8"盘状态: " << u8"已用 " << diskus << u8"GB, 可用 " << to_GB(avfree) << u8"GB, 使用率: " << diskur << "%");
			PR(u8"当前共有 " << quan << u8" 个备份 发送\"qb list\"以获得更多备份信息");
			sendText("all", string("- §c此次备份存档大小: ") + ssize + "(" + to_string(this->size) + " B)");
			sendText("all", string("- §6此次备份存档路径: ") + this->path.string());
			sendText("all", string("- §e此次备份存档MD5: ") + this->md5);
			sendText("all", string("- §a此次备份的执行者: ") + this->exer.to_string());
			sendText("all", string("- §3此次备份的压缩率: ") + compr + " (原存档大小:" + SizeToString(lsize) + ")");
			sendText("all", string("- §b备份后的") + disk[0] + "盘状态: " + "已用 " + to_string(diskus) + "GB, 可用 " + to_string(to_GB(avfree)) + "GB, 使用率: " + to_string(diskur) + "%");
			sendText("all", string("§5当前共有 ") + to_string(quan) + " 个备份 发送\\\"qb list\\\"以获得更多备份信息");
			return true;
		}
		else
		{
			PR(u8"备份失败~ ([" << res << "]" << to_UTF8(ZipRetCheck(res)) << ")");
			sendText("all", "§c备份失败~ 请联系服主");
			L_ERROR(string("备份失败: (") + to_string(res) + ")" + ZipRetCheck(res));
			return false;
		}
	}

	bool Backup::Delete(Backup::Executor exer)
	{
		L_INFO(string("删除备份[") + to_string(this->onum) + "]");
		if (this->CheckDeletePermission(exer))
		{
			if (!filesystem::exists(this->path))
			{
				L_WARNING(string("- 未找到备份[") + to_string(this->onum) + "] " + this->path.string() + " 文件");
				PRWARN(u8"未找到备份[" << this->onum << "] " << this->path.string() << u8" 文件");
				PR(u8"删除备份[" << this->onum << u8"] 的记录...");
				bool drres = rec->DeleteRecord(this->onum);
				if (exer.type == Player_Type)
				{
					sendText(exer.pname, string("§e未找到备份[") + to_string(this->onum) + "] " + this->path.string() + " 文件");
					TellAdmin(string("§e未找到备份[") + to_string(this->onum) + "] " + this->path.string() + " 文件");
					sendText(exer.pname, string("§c备份[") + to_string(this->onum) + "] 记录已删除");
					TellAdmin(string("§c备份[") + to_string(this->onum) + "] 记录已删除");
				}
				return true;
			}
			else
			{
				//PR(u8"删除备份文件...");
				L_INFO("- 删除备份文件...");
				bool fsres = filesystem::remove(this->path);
				//PR(u8"删除备份记录...");
				L_INFO("- 删除备份记录...");
				bool drres = rec->DeleteRecord(this->onum);
				if (!fsres)
				{
					PRERR(u8"删除备份文件时出现了异常!!!");
					L_ERROR("- 删除备份文件时出现了异常!!!");
				}
				if (!drres)
				{
					PRERR(u8"删除备份记录时出现了异常!!!");
					L_ERROR("- 删除备份记录时出现了异常!!!");
				}
				if (fsres && drres)
				{
					PR(u8"删除成功!");
					L_INFO("- 删除成功!");
				}
				//delete this; MSVC编译正常,其他平台可能不会起作用
			}
		}
		else
		{
			sendText(exer.pname, "§c您没有权限执行此操作!!!");
			TellAdmin(string("§c") + exer.pname + " 试图执行删除操作!!!");
		}
		return false;
	}

	bool Backup::Resume(Backup::Executor exer)
	{
		PR(u8"开始回档!您可以关闭BDS进程以取消此回档(可能导致错误)");
		L_INFO("开始回档!");
		DWORD pid = GetCurrentProcessId();
		if (!filesystem::exists(this->path))
		{
			PRWARN(u8"需要回档的存档已不存在!删除记录...");
			L_WARNING("需要回档的存档已不存在!删除记录...");
			if (exer.type == Player_Type)
			{
				sendText(exer.pname, "需要回档的存档已不存在!删除记录...");
				TellAdmin("需要回档的存档已不存在!删除记录...");
			}
			rec->DeleteRecord(this->onum);
			return false;
		}
		L_INFO("打开备份...");
		/*HZIP hz = OpenZip(this->path.wstring().c_str(), 0);
		ZIPENTRY ze; 
		GetZipItem(hz, -1, &ze); 
		int numitems = ze.index;
		L_INFO("开始解压...");
		for (int i = 0; i < numitems; i++)
		{
			GetZipItem(hz, i, &ze);
			Debug{ L_INFO(string("- 解压 ") + filesystem::path(ze.name).string() + " 至 " + RESUMEDIR + filesystem::path(ze.name).string()); }
			UnzipItem(hz, i, CharToWChar((RESUMEDIR + filesystem::path(ze.name).string()).c_str()));
		}
		ZRESULT res = CloseZip(hz);*/
		if (ZR_OK)
		{
			L_INFO("解压成功(0)");
			PR(u8"解压成功!");
		}
		else
		{
			//L_ERROR(string("解压失败: ([") + to_string(res) + "]" + ZipRetCheck(res) + ")");
			//PR(u8"解压失败~ ([" << res << "]" << ZipRetCheck(res) << ")");
		}
	}

	bool Backup::CheckBackupPermission()
	{
		if (this->exer.type == Backup::Executor::Type::AutoBackup) return true;
		if (this->exer.type == Console_Type) return true;
		if (this->exer.type == Block_Type) return cfg->acb ? true : false;
		if (Is_Admin(this->exer.pname, this->exer.pxuid)) return true;
		if (cfg->backup.size() == 0) return true;
		map<string, string>::iterator iter = cfg->backup.begin();
		for (; iter != cfg->backup.end(); ++iter)
		{
			if (iter->first == this->exer.pname)
			{
				if (iter->second != this->exer.pxuid)
					return false;
				else return true;
			}
			if (iter->second == this->exer.pxuid)
			{
				if (iter->first != this->exer.pname)
					cfg->EditPermissionName(Config::PerType::Backup, this->exer.pxuid, this->exer.pname);
				return true;
			}
		}
		return false;
	}

	bool Backup::CheckDeletePermission(Backup::Executor exer)
	{
		if (exer.type == Console_Type) return true;
		if (exer.type == AutoBak_Type) return true;
		if (exer.type == Player_Type)
		{
			if (Is_Admin(exer.pname, exer.pxuid)) return true;
			if (exer.pxuid == this->exer.pxuid) return true;
		}
		return false;
	}

	bool Backup::CheckResumePermission(Backup::Executor exer)
	{
		if (exer.type == Console_Type) return true;
		if (exer.type == Player_Type)
		{
			if (cfg->back.size() == 0) return true;
			if (Is_Admin(exer.pname, exer.pxuid)) return true;
			map<string, string>::iterator iter = cfg->back.begin();
			for (; iter != cfg->back.end(); ++iter)
			{
				if (iter->first == this->exer.pname)
				{
					if (iter->second != this->exer.pxuid)
						return false;
					else return true;
				}
				if (iter->second == this->exer.pxuid)
				{
					if (iter->first != this->exer.pname)
						cfg->EditPermissionName(Config::PerType::Back, this->exer.pxuid, this->exer.pname);
					return true;
				}
			}
		}
		return false;
	}

	ZRESULT Backup::Create()
	{
		int i;
		string bpath = to_UTF8(getCustomTime(cfg->bop.c_str()));
		Directory ldir(this->lpath);
		ldir.copy_all_to(BACKUPDIR);
		Directory dir(BACKUPDIR);
		dir.dirlist();
		this->flist = dir;
		size_t flsize = this->flist.size();
		Debug{ L_INFO(string("创建 ") + bpath + " 文件..."); }
		i = 2;
		while (filesystem::exists(bpath))
		{
			size_t ppos = bpath.find_last_of(".");
			string bpath2 = bpath.substr(0, ppos) + "(" + to_string(i) + ")" + filesystem::path(bpath).extension().string();
			if (!filesystem::exists(bpath2))
			{
				bpath = bpath2;
				break;
			}
			i++;
		}
		QBZIP zip(bpath);
		L_INFO(string("开始压缩 共有 ") + to_string(flsize) + " 个文件待压缩");
		PR(u8"开始压缩 共有 " << flsize << u8" 个文件待压缩");
		sendText("all", string("§e开始压缩 共有 ") + to_string(flsize) + " 个文件待压缩");
		using FUNC = bool(QBZIP::*)(map<string, string>);
		FUNC add1 = &QBZIP::Add;
		thread th(bind(add1, ref(zip), this->flist));
		th.join();
		dir.delete_all();
		Debug{ L_INFO(string("关闭并写入 ") + bpath + " 文件..."); }
		zip.Save();
		if (filesystem::exists(bpath))
			this->path = filesystem::canonical(filesystem::path(bpath));
		return 0i64;
	}
}