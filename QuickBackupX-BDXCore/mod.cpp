// Created by JasonZYT on 2021/02/14
/*
   ___        _      _    ____             _              __  __
  / _ \ _   _(_) ___| | _| __ )  __ _  ___| | ___   _ _ __\ \/ /
 | | | | | | | |/ __| |/ /  _ \ / _` |/ __| |/ / | | | '_ \\  / 
 | |_| | |_| | | (__|   <| |_) | (_| | (__|   <| |_| | |_) /  \ 
  \__\_\\__,_|_|\___|_|\_\____/ \__,_|\___|_|\_\\__,_| .__/_/\_\
                                                     |_|        
*/
#include "pch.h"
#include "logger.h"
#include "config.h"
#include "json.h"
#include "func.h"
#include "backup.h"
#include "json/json.h"
#include "bds.hpp"
#include <regex>
#include <shellapi.h>
#pragma warning(disable:4996)

#define If_Console if (exer.type == Console_Type)
#define If_Player  if (exer.type == Player_Type)

using namespace std;

namespace QuickBackupX
{
	VA pxuid_level;
	map<Player*, bool> PlayerOnline;
	map<string, Player*> PlayerUuid;
	map<string, bool> PlayerIsOnline;
	map<unsigned, bool> fids;
	int OnlineQuan = 0;

	Logger* log = new Logger;
	Config* cfg = new Config;
	BRecJson* rec = new BRecJson;

	/*
		const char* homeProfile = "USERPROFILE";
		char homePath[1024] = { 0 };
		unsigned int pathSize = GetEnvironmentVariableA(homeProfile, homePath, 1024);
		string addir = string(homePath) + "\\";
	*/

	void dll_init()
	{
		PR(u8"QuickBackupX Loaded! Author: JasonZYT");
		cout << u8R"(
   ___        _      _    ____             _              __  __
  / _ \ _   _(_) ___| | _| __ )  __ _  ___| | ___   _ _ __\ \/ /
 | | | | | | | |/ __| |/ /  _ \ / _` |/ __| |/ / | | | '_ \\  / 
 | |_| | |_| | | (__|   <| |_) | (_| | (__|   <| |_| | |_) /  \ 
  \__\_\\____|_|\___|_|\_\____/ \__,_|\___|_|\_\\____| .__/_/\_\
                                                     |_|        
)" << endl;
		if (!filesystem::exists("./QuickBackupX/"))
		{
			if (filesystem::create_directory(filesystem::path("./QuickBackupX/")))
			{
				PR(u8"创建QuickBackupX文件夹成功!");
				L_INFO("创建QuickBackupX文件夹成功!");
			}
			else
			{
				PRERR(u8"创建QuickBackupX文件夹失败!");
				PRERR(u8"进程即将退出");
				Sleep(3000);
				throw 100;
			}
		}
		log->Open();
		log->Start();
		cfg->getConfig();
		rec->blist = rec->ListRecord();
		ofstream eula;
		eula.open(EULAFILE, ios::app | ios::out);
		L_INFO("创建EULA.txt...");
		if (!eula.is_open())
		{
			L_ERROR(string("无法创建文件: ") + EULAFILE);
			Sleep(3000);
			throw 102;
			return;
		}
		eula << 
u8R"(QuickBackupX  EULA
Copyright (C)2020-2021 JasonZYT

*使用QuickBackupX插件(以下简称“本插件”)前，您需要先在config.json中的"EULA": false更改为"EULA": true表示您已阅读并同意EULA(最终用户许可协议)
上次更新日期: 2021年2月23日

1. 免责声明 
  1.1 您理解并同意，在使用本插件的过程中造成计算机的任何损坏(包括但不限于计算机软硬件上的损坏)本插件的作者均不承担责任。
  1.2 您使用任何本插件的衍生软件(二次开发软件、使用了本插件源代码的软件/插件)所导致的任何计算机损坏(包括但不限于计算机病毒、文件损坏)和计算机资料文件(包括但不限于软件读取隐私资料)泄漏。
2. 插件说明
  2.1 本插件遵循GPLv3.0协议开源，任何使用了本插件原创的源代码(本插件所使用的第三方库除外)的软件/插件都应开放源代码(以下简称“开源”)并注明原作者。
  2.2 本插件所使用的第三方库，插件作者均已了解相关协议，且与本协议无关。
  2.3 任何人、组织、团体不得在插件(源代码)中加入对用户计算机有害的程序(包括但不限于计算机病毒程序)后发布。
  2.4 任何人、组织、团体不得将此插件作为盈利手段，在互联网上销售本插件。
  2.5 任何人、组织、团体不得在未经插件原作者(JasonZYT)的允许下商用本插件。
  2.6 任何人、组织、团体不得破解本插件。
  2.7 插件原作者保留对本插件的所有权利。
  2.8 本插件版权所有者为插件原作者。
  2.9 破解、二次创作本插件所造成的一切后果由破解者、二次创作者承担。
)";
		eula.close();
		if (!filesystem::exists(BACKUPRECFILE))
		{
			PR(u8"未找到备份记录Json: " << BACKUPRECFILE);
			ofstream fp;
			fp.open(BACKUPRECFILE, ios::app | ios::out);
			PR(u8"正尝试创建备份记录Json...");
			if (!fp.is_open())
			{
				PRERR(u8"无法创建文件: " << BACKUPRECFILE << u8" 请尝试手动创建");
				L_ERROR(string("无法创建文件: ") + BACKUPRECFILE);
				Sleep(3000);
				throw 102;
				return;
			}
			fp << "[]" << endl;
			fp.close();
		}
		if (!filesystem::exists(cfg->getBackupDir()))
		{
			if (filesystem::create_directory(filesystem::path(cfg->getBackupDir())))
			{
				L_INFO(string("创建 ") + cfg->getBackupDir() + " 文件夹成功!");
			}
			else
			{
				PRERR(u8"创建 " << cfg->getBackupDir() << " 文件夹失败! 请尝试手动创建");
				L_INFO(string("创建 ") + cfg->getBackupDir() + " 文件夹失败!!!");
				Sleep(3000);
				throw 100;
			}
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		PR(u8"感谢 TISUnion(https://www.github.com/TISUnion/QuickBackupM) 的灵感支持");
		PR(u8"感谢 ZipUtils库(https://www.codeproject.com/Articles/7530/Zip-Utils-Clean-Elegant-Simple-Cplusplus-Win) 的代码支持");
		PR(u8"感谢 JsonCPP库(https://github.com/open-source-parsers/jsoncpp) 的代码支持");
		PR(u8"感谢 OpenSSL库(https://github.com/openssl/openssl) 的代码支持");
		PR(u8"插件已启动(Version " << QBXVERSION << u8" #" << QBXVERSIONTAG << u8"##BDXC&BDX Edition#)(GitHub Repository:https://www.github.com/ST-SKYTown/QuickBackupX)");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
	}

	void dll_exit()
	{
		L_INFO("析构全局对象...(你对象,无了/doge)");
		delete log;
		delete rec;
		delete cfg;
	}
	
	enum class QBCMDT : int
	{
		ERRPAR6 = -6,
		ERRPAR5,
		ERRPAR4,
		ERRPAR3,
		ERRPAR2,
		ERRPAR1,
		Other,
		Unknown,

		Make,
		List,
		Help,
		Del,
		Reload,
		Back
	};

	enum class QBCMDParam : int
	{
		string, _int, _bool, null
	};
	
	struct QBCMD
	{
		QBCMDT type = QBCMDT::Unknown;
		vector<string> params;
		QBCMDParam ParamType(int pnum)
		{
			string param = params[pnum];
			regex reg("^([0-9]+)$");
			if (regex_match(param, reg))
				return QBCMDParam::_int;
			if (param == "true" || param == "false")
				return QBCMDParam::_bool;
			if (param == "")
				return QBCMDParam::null;
			return QBCMDParam::string;
		}
	};

	QBCMD CMDCheck(string cmd, Backup::Executor exer)
	{
		QBCMD rv;
		vector<string> params = split(cmd);
		int paramsize = params.size();
		if (paramsize == 0)
		{
			rv.type = QBCMDT::Other; 
			return rv;
		}
		rv.params = params;
		if (params[0] == "qb")
		{
			if (paramsize == 1)
			{
				rv.type = QBCMDT::Help;
				return rv;
			}
			if (params[1] == "make")
			{
				if (paramsize == 2) rv.type = QBCMDT::Make;
				else
				{
					If_Console{ PRERR(u8"意外的 " << params[2] << u8" 出现在 " << cmd); }
					If_Player{ sendText(exer.pname,string("§c意外的 ") + params[2] + " 出现在 " + cmd); }
					L_ERROR(string("- 意外的 ") + params[2] + " 出现在 " + cmd);
					rv.type = QBCMDT::ERRPAR2;
				}
			}
			else if (params[1] == "list")
			{
				if (paramsize == 2) rv.type = QBCMDT::List;
				else if (paramsize > 3)
				{
					If_Console{ PRERR(u8"意外的 " << params[4] << u8" 出现在 " << cmd); }
					If_Player{ sendText(exer.pname,string("§c意外的 ") + params[4] + " 出现在 " + cmd); }
					L_ERROR(string("- 意外的 ") + params[4] + " 出现在 " + cmd);
					rv.type = QBCMDT::ERRPAR3;
				}
				else if (rv.ParamType(2) == QBCMDParam::_int) rv.type = QBCMDT::List;
				else { 
					If_Console{ PRERR(u8"参数 [page: int](页码) 不合法!"); }
					If_Player{ sendText(exer.pname, "§c参数 [page: int](页码) 不合法!"); }
					L_ERROR("参数 [page: int](页码) 不合法!");
					rv.type = QBCMDT::ERRPAR2; 
				}
			}
			else if (params[1] == "del")
			{
				if (paramsize == 2)
				{
					If_Console{ PRERR(u8"参数 [onum: int](序号) 不合法!"); }
					If_Player{ sendText(exer.pname, "§c参数 [onum: int](序号) 不合法!"); }
					L_ERROR("参数 [onum: int](序号) 不合法!");
					rv.type = QBCMDT::ERRPAR2;
				}
				else if (paramsize == 3 && rv.ParamType(2) == QBCMDParam::_int) rv.type = QBCMDT::Del;
				else if (paramsize == 3 && params[2] == "all") rv.type = QBCMDT::Del;
				else if (paramsize == 3)
				{
					If_Console{ PRERR(u8"参数 [onum: int](序号) 不合法!"); }
					If_Player{ sendText(exer.pname, "§c参数 [onum: int](序号) 不合法!"); }
					L_ERROR("参数 [onum: int](序号) 不合法!");
					rv.type = QBCMDT::ERRPAR2;
				}
				else
				{
					If_Console{ PRERR(u8"意外的 " << params[4] << u8" 出现在 " << cmd); }
					If_Player{ sendText(exer.pname,string("§c意外的 ") + params[4] + " 出现在 " + cmd); }
					L_ERROR(string("- 意外的 ") + params[4] + " 出现在 " + cmd);
					rv.type = QBCMDT::ERRPAR3;
				}
			}
			else if (params[1] == "reload")
			{
				if (paramsize == 2) rv.type = QBCMDT::Reload;
				else
				{
					If_Console{ PRERR(u8"意外的 " << params[2] << u8" 出现在 " << cmd); }
					If_Player{ sendText(exer.pname,string("§c意外的 ") + params[2] + " 出现在 " + cmd); }
					L_ERROR(string("- 意外的 ") + params[2] + " 出现在 " + cmd);
					rv.type = QBCMDT::ERRPAR2;
				}
			}
			else if (params[1] == "help")
			{
				if (paramsize == 2) rv.type = QBCMDT::Help;
				else
				{
					If_Console{ PRERR(u8"意外的 " << params[2] << u8" 出现在 " << cmd); }
					If_Player{ sendText(exer.pname,string("§c意外的 ") + params[2] + " 出现在 " + cmd); }
					L_ERROR(string("- 意外的 ") + params[2] + " 出现在 " + cmd);
					rv.type = QBCMDT::ERRPAR2;
				}
			}
			else if (params[1] == "back")
			{
				if (paramsize == 2)
				{
					If_Console{ PRERR(u8"参数 [onum: int](序号) 不合法!"); }
					If_Player{ sendText(exer.pname, "§c参数 [onum: int](序号) 不合法!"); }
					L_ERROR("参数 [onum: int](序号) 不合法!");
					rv.type = QBCMDT::ERRPAR2;
				}
				else if (paramsize == 3)
				{
					if (rv.ParamType(2) != QBCMDParam::_int)
					{
						If_Console{ PRERR(u8"参数 [onum: int](序号) 不合法!"); }
						If_Player{ sendText(exer.pname, "§c参数 [onum: int](序号) 不合法!"); }
						L_ERROR("参数 [onum: int](序号) 不合法!");
						rv.type = QBCMDT::ERRPAR3;
					}
					else QBCMDT::Back;
				}
				else if (paramsize == 4)
				{
					if (rv.ParamType(2) != QBCMDParam::_int)
					{
						If_Console{ PRERR(u8"参数 [onum: int](序号) 不合法!"); }
						If_Player{ sendText(exer.pname, "§c参数 [onum: int](序号) 不合法!"); }
						L_ERROR("参数 [onum: int](序号) 不合法!");
						rv.type = QBCMDT::ERRPAR3;
					}
					else if (rv.ParamType(3) != QBCMDParam::_bool)
					{
						If_Console{ PRERR(u8"参数 [delsrc: bool](是否删除源) 不合法!"); }
						If_Player{ sendText(exer.pname, "§c参数 [delsrc: bool](是否删除源) 不合法!"); }
						L_ERROR("参数 [delsrc: bool](是否删除源) 不合法!");
						rv.type = QBCMDT::ERRPAR4;
					}
					else QBCMDT::Back;
				}
				else if (paramsize == 5)
				{
					if (rv.ParamType(2) != QBCMDParam::_int)
					{
						If_Console{ PRERR(u8"参数 [onum: int](序号) 不合法!"); }
						If_Player{ sendText(exer.pname, "§c参数 [onum: int](序号) 不合法!"); }
						L_ERROR("参数 [onum: int](序号) 不合法!");
						rv.type = QBCMDT::ERRPAR3;
					}
					else if (rv.ParamType(3) != QBCMDParam::_bool)
					{
						If_Console{ PRERR(u8"参数 [delsrc: bool](是否删除源) 不合法!"); }
						If_Player{ sendText(exer.pname, "§c参数 [delsrc: bool](是否删除源) 不合法!"); }
						L_ERROR("参数 [delsrc: bool](是否删除源) 不合法!");
						rv.type = QBCMDT::ERRPAR4;
					}
					else if (rv.ParamType(4) != QBCMDParam::_bool) 
					{
						If_Console{ PRERR(u8"参数 [delevel: bool](是否删除存档) 不合法!"); }
						If_Player{ sendText(exer.pname, "§c参数 [delevel: bool](是否删除存档) 不合法!"); }
						L_ERROR("参数 [delevel: bool](是否删除存档) 不合法!");
						rv.type = QBCMDT::ERRPAR5;
					}
					else QBCMDT::Back;
				}
				else
				{
					If_Console{ PRERR(u8"意外的 " << params[6] << u8" 出现在 " << cmd); }
					If_Player{ sendText(exer.pname,string("§c意外的 ") + params[6] + " 出现在 " + cmd); }
					L_ERROR(string("- 意外的 ") + params[6] + " 出现在 " + cmd);
					rv.type = QBCMDT::ERRPAR6;
				}
			}
			else
			{
				If_Console{ PRERR(u8"参数 <cmd: QBCMDT>(命令) 不合法!"); }
				If_Player{ sendText(exer.pname, "§c参数 <cmd: QBCMDT>(命令) 不合法!"); }
				L_ERROR("参数 <cmd: int>(命令) 不合法!");
				rv.type = QBCMDT::ERRPAR1;
			}
		}
		else
		{
			rv.type = QBCMDT::Other;
		}
		return rv;
	}

	SYMHOOK(onServerCMD, bool, "??$inner_enqueue@$0A@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@?$SPSCQueue@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$0CAA@@@AEAA_NAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z", VA _this, string* cmd)
	{
		string cmdstr = *cmd;
		Backup::Executor exer;
		exer.type = Console_Type;
		QBCMD qcmd = CMDCheck(cmdstr, exer);
		//cout << (int)qcmd.type;
		if (qcmd.type != QBCMDT::Other)
		{
			L_INFO(string("控制台执行了QB命令: ") + *cmd);
			if (qcmd.type == QBCMDT::Make)
			{
				// Backup Main
				Backup* bak = new Backup;
				thread mkbackup(&Backup::Make, bak, exer); 
				mkbackup.detach(); // "detach以后，子线程会成为孤儿线程，线程之间将无法通信"
				//bak->Make(exer);
			}
			else if (qcmd.type == QBCMDT::List)
			{
				int page;
				if (qcmd.params.size() == 2) page = 1;
				else page = atoi(qcmd.params[2].c_str());
				int pag = 1;
				if (page > 0) pag = page;
				// List Main
				vector<Backup*> baklist = rec->blist;
				if (cfg->lops <= 0)
				{
					for (int iter = 0; iter <= baklist.size() - 1 ; iter++)
					{
						cout << u8"- 备份[" << iter + 1 << "] " << baklist[iter]->time << " " << SizeToString(baklist[iter]->size) << endl;
					}
					return false;
				}
				int lpages = cfg->lops;
				double v1 = (baklist.size() / static_cast<double>(lpages));
				int page_quan = v1 + 0.99999999;
				if (pag > page_quan) pag = page_quan;
				if (page_quan == 0)
				{
					PRWARN(u8"无备份!!! 输入 \"qb make\" 创建一个备份");
					return false;
				}
				int startline = lpages * (pag - 1);
				int endline = lpages * pag - 1;
				cout << u8"========================= 备份列表 第 " << pag << "/" << page_quan << u8" 页 共 " << baklist.size() << u8" 个备份 =========================" << endl;
				for (int iter = startline; iter <= endline && iter <= baklist.size() - 1; iter++)
				{
					cout << u8"- 备份[" << iter + 1 << "] " << baklist[iter]->time << " " << SizeToString(baklist[iter]->size) << endl;
				}
				cout << u8"========================= 备份列表 第 " << pag << "/" << page_quan << u8" 页 共 " << baklist.size() << u8" 个备份 =========================" << endl;
			}
			else if (qcmd.type == QBCMDT::Del)
			{
				if (qcmd.params[2] == "all")
				{
					PRWARN(u8"您正在执行删除全部备份!");
				}
				int onum = atoi(qcmd.params[2].c_str());
				if (onum >= rec->blist.size())
				{
					PRERR(u8"找不到备份[" << onum << "]!发送\"qb list\"查看当前的备份");
					L_ERROR("- 执行失败!");
					return false;
				}
				Backup::Executor exer;
				exer.type = Console_Type;
				rec->blist[(onum - 1)]->Delete(exer);
			}
			else if (qcmd.type == QBCMDT::Reload)
			{
				if (cfg->getConfig())
				{
					PR(u8"配置重载成功!");
					L_INFO("配置重载成功!");
				}
				else
				{
					PR(u8"配置重载失败: " << CONFIGFILE << "无法打开!!!");
					L_INFO("配置重载失败!");
				}
			}
			else if (qcmd.type == QBCMDT::Back)
			{
				
			}
			return false;
		}
		original(_this, cmd);
	}

	/*
	SYMHOOK(onPlayerCMD, void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVCommandRequestPacket@@@Z", VA _this, VA id, CommandRequestPacket* crp)
	{
		Player* pp = SYMCALL<Player*>("?_getServerPlayer@ServerNetworkHandler@@AEAAPEAVServerPlayer@@AEBVNetworkIdentifier@@E@Z", _this, id, *((char*)crp + 16));
		string cmd = crp->toString();
		Backup::Executor exer;
		exer.type = Player_Type;
		exer.pname = pp->getNameTag();
		exer.pxuid = pp->getXuid(pxuid_level);
		QBCMD qcmd = CMDCheck(cmd, exer);
		if (qcmd.type != QBCMDT::Other)
		{
			L_INFO(string("玩家 ") + pp->getNameTag() + " 执行了QB命令: " + cmd);
			if (qcmd.type == QBCMDT::Make)
			{
				// Backup Main
				Backup* bak = new Backup;
				thread mkbackup(&Backup::Make, bak, exer);
				mkbackup.detach(); // "detach以后，子线程会成为孤儿线程，线程之间将无法通信"
				//bak->Make(exer);
			}
			else if (qcmd.type == QBCMDT::List)
			{
				int page;
				if (qcmd.params.size() == 2) page = 1;
				else
				{
					if (qcmd.ParamType(2) == QBCMDParam::_int)
					{
						sendText(pp->getNameTag(), "参数 [page: int](页码) 不合法,请重试!");
						L_ERROR("- 参数 [page: int](页码) 不合法！");
						L_ERROR("- 执行失败!");
						return;
					}
					page = atoi(qcmd.params[2].c_str());
				}
				int pag = 1;
				if (page > 0) pag = page;
				// List Main
				vector<Backup*> baklist = rec->blist;
				double v1 = (baklist.size() / static_cast<double>(30));
				int page_quan = ceil(v1);
				if (pag > page_quan) pag = page_quan;
				if (page_quan == 0)
				{
					PRWARN(u8"无备份!!! 输入 \"qb make\" 创建一个备份");
					return;
				}
				int startline = 30 * (pag - 1);
				int endline = 30 * pag - 1;
				cout << u8"========================= 备份列表 第 " << pag << "/" << page_quan << u8" 页 共 " << baklist.size() << u8" 个备份 =========================" << endl;
				for (int iter = startline; iter <= endline && iter <= baklist.size() - 1; iter++)
				{
					cout << u8"- 备份[" << iter + 1 << "] " << baklist[iter]->time << " " << SizeToString(baklist[iter]->size) << endl;
				}
				cout << u8"========================= 备份列表 第 " << pag << "/" << page_quan << u8" 页 共 " << baklist.size() << u8" 个备份 =========================" << endl;
			}
			else if (qcmd.type == QBCMDT::Del)
			{
				if (qcmd.ParamType(2) != QBCMDParam::_int)
				{
					PRERR(u8"参数 [onum: int](备份序号) 不合法,请重试!");
					L_ERROR("- 参数 [onum: int](备份序号) 不合法！");
					L_ERROR("- 执行失败!");
					return;
				}
				int onum = atoi(qcmd.params[2].c_str());
				rec->blist[(onum - 1)]->Delete(exer);
			}
			else
			{
				PRERR(u8"参数不合法!");
				L_ERROR("- 参数不合法！");
				L_ERROR("- 执行失败!");
				return;
			}
			return;
		}
		original(_this, id, crp);
	}*/

	SYMHOOK(onCBCMD, bool, "?performCommand@CommandBlockActor@@QEAA_NAEAVBlockSource@@@Z",
		VA _this, BlockSource* a2) {
		//脉冲:0,重复:1,链:2
		int mode = SYMCALL<int>("?getMode@CommandBlockActor@@QEBA?AW4CommandBlockMode@@AEAVBlockSource@@@Z",
			_this, a2);
		//无条件:0,有条件:1
		bool condition = SYMCALL<bool>("?getConditionalMode@CommandBlockActor@@QEBA_NAEAVBlockSource@@@Z",
			_this, a2);
		string cmd = offset(string, _this + 264/*208*/);
		//string rawname = offset(string, _this + 296);
		BlockPos bp = offset(BlockPos, _this + 44);
		int dim = a2->getDimensionId();
		Backup::Executor exer;
		exer.type = Block_Type;
		exer.cbmode = (Backup::Executor::CBMode)mode;
		exer.cbcdit = condition;
		exer.cbpos = to_string(dim) + ":" + bp.toString(); // 1:0,0,0 地狱0,0,0处
		QBCMD qcmd = CMDCheck(cmd, exer);
		if (qcmd.type != QBCMDT::Other)
		{
			if (qcmd.type == QBCMDT::Make)
			{
				Backup* bak = new Backup;
				bak->Make(exer);
			}
			return false;
		}
		original(_this, a2);
	}

	SYMHOOK(GetXuid, Player*, "??0Player@@QEAA@AEAVLevel@@AEAVPacketSender@@W4GameType@@AEBVNetworkIdentifier@@EVUUID@mce@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$unique_ptr@VCertificate@@U?$default_delete@VCertificate@@@std@@@8@55@Z", Player* _this, VA level, __int64 a3, int a4, __int64 a5, __int64 a6, void* xuid, std::string& strxuid, __int64* a9, __int64 a10, __int64 a11) {
		pxuid_level = level;
		return original(_this, level, a3, a4, a5, a6, xuid, strxuid, a9, a10, a11);
	}

	SYMHOOK(onServerCMDOutput, VA, "??$_Insert_string@DU?$char_traits@D@std@@_K@std@@YAAEAV?$basic_ostream@DU?$char_traits@D@std@@@0@AEAV10@QEBD_K@Z", VA handle, char* str, VA size)
	{
		string output = string(str);
		if (output == "MCPE")
		{
			//HRESULT res = URLDownloadToFileA(NULL, "plugin.skytown.xyz", "./QuickBackupX/qbx.exe", NULL, NULL);
		}
		return original(handle, str, size);
	}

	SYMHOOK(SPSCQueue, VA, "??0?$SPSCQueue@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$0CAA@@@QEAA@_K@Z",
		VA _this) {
		p_spscqueue = original(_this);
		return p_spscqueue;
	}

	SYMHOOK(onPlayerJoined, VA, "?onPlayerJoined@ServerScoreboard@@UEAAXAEBVPlayer@@@Z", VA a1, Player* p)
	{
		OnlineQuan++;
		PlayerOnline[p] = true;
		PlayerIsOnline[p->getNameTag()] = true;
		PlayerUuid[p->getUuid()->toString()] = p;
		return original(a1, p);
	}
	
	SYMHOOK(onPlayerLeft, void, "?_onPlayerLeft@ServerNetworkHandler@@AEAAXPEAVServerPlayer@@_N@Z", VA _this, Player* p, char v3)
	{
		OnlineQuan--;
		PlayerOnline[p] = false;
		PlayerIsOnline[p->getNameTag()] = false;
		PlayerUuid.erase(p->getUuid()->toString());
		return original(_this, p, v3);
	}
}