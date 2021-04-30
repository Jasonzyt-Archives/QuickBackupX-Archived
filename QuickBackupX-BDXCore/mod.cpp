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
#include "record.h"
#include "func.h"
#include "backup.h"
#include "json/json.h"
#include "bds.hpp"
#include "autobackup.h"
#include "qbzip.h"
#include <mc/Command/CommandReg.h>
#include <mc/Command/Command.h>
#include <mc/Player.h>
#include <loader/Loader.h>
#include <api/commands.h>
#include <api/types/types.h>
#include <api/Basic_Event.h>
#include <shellapi.h>
#pragma warning(disable:4996)

#define Tell_P(...) sendText(exer.pname, __VA_ARGS__)
#define If_Console if (exer.type == Console_Type)
#define If_Player  if (exer.type == Player_Type)

using namespace std;

namespace QuickBackupX
{
	VA pxuid_level, p_spscqueue;
	map<Player*, bool> PlayerOnline;
	map<string, Player*> PlayerUuid;
	map<string, bool> PlayerIsOnline;
	map<string, string> PlayerXuid;
	map<unsigned, bool> fids;
	int OnlineQuan = 0;

	Logger* log = new Logger;
	Config* cfg = new Config;
	BRecord* rec = new BRecord;
	AutoBackup* ab = new AutoBackup;

	void dll_init();
	void dll_exit();
	void RunAutoBackup()
	{
		L_INFO("AutoBackup线程开始...");
		while (true)
		{
			ab->Run(); // 每分钟试图执行一次
			this_thread::sleep_for(chrono::milliseconds(60000));
		}
	}
	bool MakeBackup(Backup* bak, Backup::Executor exer)
	{
		bool rv = bak->Make(exer);
		delete bak;
		return rv;
	}

#pragma region Command_Helper
	string cmd_helper_nc = R"(QuickBackupX Command Helper
前往https://github.com/Jasonzyt/QuickBackupX/wiki查看命令详解体验更佳
## `make`
执行一次备份操作
### 语法
1. `qb make [note: string]` 
### 参数
- 参数1-1: 备份备注
  - 类型 `字符串`
  - 省略 可(默认为"这是一个备份")
## `list`
列出备份
### 语法
1. `qb list [page: int]` 
### 参数
- 参数1-1: 页数(每页的数量取决于配置)
  - 类型 `整数类型`
  - 省略 可(默认为1)
## `view` ???
## `del`
删除备份
### 语法
1. `qb del [id: int]` 逐个删除备份
2. `qb del all` 删除所有备份
### 参数
- 参数1-1: 备份序号
  - 类型 `整数`
  - 省略 不可
## `auto`
### 语法
1. `qb auto [auto_op: enum]`
2. ???
### 参数
- 参数1-1: 自动备份操作类型
  - 类型 `枚举`
  - 省略 可(默认为query)
  - 可选值 on(开启),off(关闭),list(列出详情),query(查询)
## `help`
获取帮助(≈`help qb`)
### 语法
1. `qb help`)";
#pragma endregion

#pragma region Command_ENUMS
	enum class Auto_Operation_1 : int
	{
		off = 1, 
		on = 2,
		list = 3,
		cancel = 4
	};
	enum class Auto_Operation_2 : int
	{
		add = 1
	};
	enum class QBCMD_1 : int
	{
		Help = 1, // qb help
		Reld = 2, // qb reload
		Del_All = 3
	};
	enum class QBCMD_2I1 : int 
	{
		View = 1, // qb view [Order Num: int]
		Del  = 2, // qb del  [Order Num: int]
	};
	enum class QBCMD_2I2 : int 
	{
		List = 1, // qb list [Order Num: int]
	};
	enum class QBCMD_2S : int
	{
		Make = 1  // qb make <Note: string>
	};
	enum class QBCMD_2A : int
	{
		Auto = 1  // qb auto <Operate: Auto_Operation_1>
	};
	enum class QBCMD_3 : int
	{
		Auto = 1  // qb auto <Operate: Auto_Operation_1> <Time: string>
	};
	enum class QBCMD_4 : int
	{
		Back = 1  // qb back [Order Num: int] [DelSrc: bool] [DelLvl: bool]
	};
#pragma endregion

#pragma region Command_Functions
	namespace QBCMD
	{
		bool MakeCmd(CommandOrigin const& ori, CommandOutput& outp, string note = "")
		{
			Backup::Executor exer;
			if (note.size() >= 200)
			{
				outp.error("备注字数超过限制!!! 已清空备注");
				note.clear();
			}
			switch (ori.getOriginType())
			{
			case OriginType::Player:
			{
				string pnam = ori.getName();
				exer.type = Player_Type;
				exer.pname = pnam;
				exer.pxuid = PlayerXuid[pnam];
				thread mkbackup(&MakeBackup, new Backup(note), exer);
				mkbackup.detach();
				break;
			}
			case OriginType::DedicatedServer:
			{
				exer.type = Console_Type;
				thread mkbackup(&MakeBackup, new Backup(note), exer);
				mkbackup.detach();
				break;
			}
			case OriginType::Block:
			{
				exer.type = Block_Type;
				::BlockPos bp = ori.getBlockPosition();
				int dimid = dAccess<int, 216>(ori.getDimension());
				ori.getDimension();
				Backup::Executor exer;
				exer.type = Block_Type;
				exer.cbpos = to_string(dimid) + ":" + bp.toString(); // 1:0,0,0 地狱0,0,0处
				thread mkbackup(&MakeBackup, new Backup(note), exer);
				mkbackup.detach();
				break;
			}
			}
		}
		bool ReldCmd(CommandOrigin const& ori, CommandOutput& outp)
		{
			switch (ori.getOriginType())
			{
			case OriginType::Player:
			{
				outp.error("§c您没有权限执行此操作!!! 此操作所需最低权限:Console");
				break;
			}
			case OriginType::DedicatedServer:
			{
				if (cfg->getConfig())
				{
					ab->Init();
					rec->RefrashRecord();
					outp.success("配置重载成功!");
					L_INFO("配置重载成功!");
				}
				else
				{
					outp.error("配置重载失败: " + CONFIGFILE + "无法打开!!!");
					L_INFO("配置重载失败!");
				}
				break;
			}
			}
			return true;
		}
		bool ListCmd(CommandOrigin const& ori, CommandOutput& outp, int page)
		{
			// TODO: 自定义格式化输出
			switch (ori.getOriginType())
			{
			case OriginType::Player:
			{
				ostringstream out;
				if (cfg->lops <= 0)
				{
					for (auto& it : rec->blist)
					{
						out << "§e- 备份[" << it->id + 1 << "] " << it->time << " " << SizeToString(it->size) << endl;
					}
					outp.success(out.str());
					return true;
				}
				int lpages = cfg->lops;
				double v1 = (rec->blist.size() / SC(double, lpages));
				int page_quan = v1 + 0.99999999;
				if (page > page_quan) page = page_quan;
				if (page_quan == 0)
				{
					outp.error("§c无备份!!! 输入 \"qb make\" 创建一个备份");
					return false;
				}
				int startline = lpages * (page - 1);
				int endline = lpages * page - 1;
				out << "§b========================= 备份列表 第 " << page << "/" << page_quan << " 页 共 " << rec->blist.size() << " 个备份 =========================" << endl;
				for (int iter = startline; iter <= endline && iter <= rec->blist.size() - 1; iter++)
				{
					out << "§e- 备份[" << iter + 1 << "] " << rec->blist[iter]->time << " " << SizeToString(rec->blist[iter]->size) << endl;
				}
				out << "§b========================= 备份列表 第 " << page << "/" << page_quan << " 页 共 " << rec->blist.size() << " 个备份 =========================" << endl;
				outp.success(out.str());
				break;
			}
			case OriginType::DedicatedServer:
			{
				ostringstream out;
				if (cfg->lops <= 0)
				{
					for (auto& it : rec->blist)
					{
						out << "- 备份[" << it->id << "] " << it->time << " " << SizeToString(it->size) << endl;
					}
					outp.success(out.str());
					return true;
				}
				int lpages = cfg->lops;
				double v1 = (rec->blist.size() / SC(double, lpages));
				int page_quan = v1 + 0.99999999;
				if (page > page_quan) page = page_quan;
				if (page_quan == 0)
				{
					outp.error("无备份!!! 输入 \"qb make\" 创建一个备份");
					return false;
				}
				int startline = lpages * (page - 1);
				int endline = lpages * page - 1;
				out << "========================= 备份列表 第 " << page << "/" << page_quan << u8" 页 共 " << rec->blist.size() << u8" 个备份 =========================" << endl;
				for (int iter = startline; iter <= endline && iter <= rec->blist.size() - 1; iter++)
				{
					out << "- 备份[" << iter + 1 << "] " << rec->blist[iter]->time << " " << SizeToString(rec->blist[iter]->size) << endl;
				}
				out << "========================= 备份列表 第 " << page << "/" << page_quan << u8" 页 共 " << rec->blist.size() << u8" 个备份 =========================" << endl;
				outp.success(out.str());
				break;
			}
			}
			return true;
		}
		bool DeleteCmd(CommandOrigin const& ori, CommandOutput& outp, int id)
		{
			Backup::Executor exer;
			switch (ori.getOriginType())
			{
			case OriginType::DedicatedServer:
				exer.type = Console_Type;
				if (id >= rec->blist.size())
				{
					outp.error("找不到备份[" + to_string(id) + "]!发送\"qb list\"查看当前的备份");
					L_ERROR("- 执行失败!");
					return false;
				}
				rec->blist[(id - 1)]->Delete(exer);
				break;
			case OriginType::Player:
				string pnam = ori.getName();
				exer.type = Player_Type;
				exer.pname = pnam;
				exer.pxuid = PlayerXuid[pnam];
				if (id >= rec->blist.size())
				{
					outp.error("§c找不到备份[" + to_string(id) + "]!发送\"qb list\"查看当前的备份");
					L_ERROR("- 执行失败!");
					return false;
				}
				rec->blist[(id - 1)]->Delete(exer);
			}
		}
		bool DeleteAllCmd(CommandOrigin const& ori, CommandOutput& outp)
		{
			switch (ori.getOriginType())
			{
			case OriginType::DedicatedServer:
			{
				Backup::Executor exer;
				exer.type = Console_Type;
				if (rec->blist.size() == 0)
				{
					outp.error("无备份!!! 输入 \"qb make\" 创建一个备份");
					return false;
				}
				outp.addMessage("您正在执行删除全部备份!");
				L_WARNING("执行删除全部备份...");
				vector<Backup*> blist = rec->blist;
				for (auto& it : blist)
				{
					int id = it->id;
					outp.addMessage("删除备份[" + to_string(id) + "]...");
					it->Delete(exer);
				}
				outp.success("删除完成!");
				L_INFO(u8"删除完成!");
				break;
			}
			case OriginType::Player:
				outp.error("§c您没有权限执行此操作!!! 此操作所需最低权限:Console");
			}
			return true;
		}
		bool AutoCmdQuery(CommandOrigin const& ori, CommandOutput& outp)
		{
			switch (ori.getOriginType())
			{
			case OriginType::DedicatedServer:
				outp.success(string("当前的备份状态为: ") + (ab->is_on ? "On" : "Off"));
				break;
			case OriginType::Player:
				outp.success(string("§b当前的备份状态为: ") + (ab->is_on ? "§aOn" : "§cOff"));
				break;
			}
			return true;
		}
		bool AutoCmdOp(CommandOrigin const& ori, CommandOutput& outp, bool ison)
		{
			switch (ori.getOriginType())
			{
			case OriginType::DedicatedServer:
				ab->is_on = ison;
				AutoCmdQuery(ori, outp);
				break;
			case OriginType::Player:
				ab->is_on = ison;
				AutoCmdQuery(ori, outp);
				break;
			}
			return true;
		}
		bool AutoCmdList(CommandOrigin const& ori, CommandOutput& outp)
		{
		/*	这部分先咕了
			ostringstream osstr;
			osstr << u8"当前的备份状态为: " << (ab->is_on ? "On" : "Off") << endl
				<< u8"将会在以下时间执行自动备份: ";
			for (auto& _t : ab->time)
			{
				tm* ltime = localtime(&_t);
				osstr << ltime->tm_hour << ":" << ltime->tm_min << " ";
			}
			osstr << endl;
			if (ab->is_on)
			{
				tm tmnow;
				vector<int> dtres;
				tmnow.tm_hour = ab->getLocalHour();
				tmnow.tm_min = ab->getLocalMinute();
				time_t tmtn = mktime(&tmnow);
				for (auto& _t1 : ab->time)
				{
					dtres.push_back(difftime(tmtn, _t1));
				}
				sort(dtres.begin(), dtres.end());
				int h = dtres[0] / 60 / 60;
				int m = dtres[0] - 60 * h;
				osstr << u8"距离下次自动备份还有: " << h << u8" 小时 " << m << u8" 分钟" << endl;
			}
			cout << osstr.str();
		*/
			switch (ori.getOriginType())
			{
			case OriginType::DedicatedServer:
				break;
			case OriginType::Player:
				break;
			}
		}
		bool HelpCmd(CommandOrigin const& ori, CommandOutput& outp)
		{
			// 咕咕咕
		}
#pragma endregion

#pragma region Command_Register
		bool onCMD_0(CommandOrigin const& ori, CommandOutput& outp) {}
		bool onCMD_1(CommandOrigin const& ori, CommandOutput& outp, MyEnum<QBCMD_1>& op) 
		{
			switch (op.val)
			{
			case QuickBackupX::QBCMD_1::Help:
				outp.success(cmd_helper_nc);
				break;
			case QuickBackupX::QBCMD_1::Reld:
				ReldCmd(ori, outp);
				break;
			case QuickBackupX::QBCMD_1::Del_All:
				DeleteAllCmd(ori, outp);
				break;
			}
			return true;
		}
		bool onCMD_2S(CommandOrigin const& ori, CommandOutput& outp, MyEnum<QBCMD_2S>& op, optional<string>& pstr)
		{
			switch (op.val)
			{
			case QuickBackupX::QBCMD_2S::Make:
				if (!pstr.set) MakeCmd(ori, outp);
				else MakeCmd(ori, outp, pstr.val());
				break;
			}
			return true;
		}
		bool onCMD_2I1(CommandOrigin const& ori, CommandOutput& outp, MyEnum<QBCMD_2I1>& op, int& pint)
		{
			switch (op)
			{
			case QuickBackupX::QBCMD_2I1::View:
				outp.addMessage("唔...此功能还在开发中呢");
				break;
			case QuickBackupX::QBCMD_2I1::Del:
				DeleteCmd(ori, outp, pint);
				break;
			}
			return true;
		}
		bool onCMD_2I2(CommandOrigin const& ori, CommandOutput& outp, MyEnum<QBCMD_2I2>& op, optional<int>& pint)
		{
			switch (op)
			{
			case QuickBackupX::QBCMD_2I2::List:
				if (pint.set == false) ListCmd(ori, outp, 1);
				else ListCmd(ori, outp, pint.val());
				break;
			}
			return true;
		}
		bool onCMD_2A(CommandOrigin const& ori, CommandOutput& outp, MyEnum<QBCMD_2A>& op, MyEnum<Auto_Operation_1>& ao)
		{
			switch (op)
			{
			case QBCMD_2A::Auto:
			{
				switch (ao)
				{
				case Auto_Operation_1::on:
					AutoCmdOp(ori, outp, true);
					break;
				case Auto_Operation_1::off:
					AutoCmdOp(ori, outp, false);
					break;
				case Auto_Operation_1::cancel:
					ab->cancel = true;
					break;
				}
				break;
			}
			}
			return true;
		}
		bool onCMD_Test(CommandOrigin const& ori, CommandOutput& outp)
		{
			QBZIP qz1("./testAdd.zip");
			qz1.Init();
			qz1.Add("./test.txt", "114514\n1919810");
			qz1.Add("./test2.txt", "啊2114514\n1919810");
			qz1.Save();
			outp.addMessage("Test FUNC[QBZIP::Add] Done");
			QBZIP qz2("./testAddFile.zip");
			qz2.Init();
			qz2.AddFile("./test.new.txt", "test.new.txt");
			qz2.AddFile("./usage.txt", "usage.txt");
			qz2.AddFile("./server.properties", "server.properties");
			qz2.Save();
			outp.addMessage("Test FUNC[QBZIP::AddFile] Done");
			FList fl;
			fl.insert(pair<string, string>("usage.txt", "usage.txt"));
			fl.insert(pair<string, string>("RoDB.exe", "RoDB.exe"));
			fl.insert(pair<string, string>("QuickBackupX/qbx.log", "qbx/qbx.log"));
			QBZIP qz3("./testAddFList.zip");
			qz3.Init();
			qz3.AddFList(fl);
			qz3.Save();
		}
	}
#pragma endregion

	void dll_init()
	{
		PR(u8"QuickBackupX Loaded! Author: JasonZYT");
		cout << 
u8R"(   ___        _      _    ____             _              __  __
  / _ \ _   _(_) ___| | _| __ )  __ _  ___| | ___   _ _ __\ \/ /
 | | | | | | | |/ __| |/ /  _ \ / _` |/ __| |/ / | | | '_ \\  / 
 | |_| | |_| | | (__|   <| |_) | (_| | (__|   <| |_| | |_) /  \ 
  \__\_\\____|_|\___|_|\_\____/ \__,_|\___|_|\_\\____| .__/_/\_\
                                                     |_|        
)" << endl;
		log->Open();
		log->Start();
		cfg->getConfig();
		rec->blist = move(rec->ListRecord());
		ab->Init();
#pragma region File_Check
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
		ofstream eula;
		eula.open(EULAFILE, ios::ate | ios::out);
		if (!eula.is_open())
		{
			L_ERROR(string("无法打开文件: ") + EULAFILE);
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
			if (filesystem::create_directories(filesystem::path(cfg->getBackupDir())))
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
#pragma endregion
		Event::addEventListener([](RegCmdEV ev) {
			CMDREG::SetCommandRegistry(ev.CMDRg);
			CEnum<QBCMD_1> _1("QBCMD1", { "help","reload" });
			CEnum<QBCMD_3> _3("QBCMD3", { "auto" });
			CEnum<QBCMD_4> _4("QBCMD4", { "back" });
			CEnum<QBCMD_2S> _2a("QBCMD2S", { "make" });
			CEnum<QBCMD_2A> _2c("QBCMD2A", { "auto" });
			CEnum<QBCMD_2I1> _2b("QBCMD2I1", { "del","view" });
			CEnum<QBCMD_2I2> _2e("QBCMD2I2", { "list" });
			CEnum<Auto_Operation_1> _a1("Auto_1", { "on","off","list","cancel" });
			CEnum<Auto_Operation_2> _a2("Auto_2", { "add" });
			MakeCommand("qb", "QuickBackupX Plugin Command", 0);
			CmdOverload(qb, QBCMD::onCMD_1, "CMD");
			CmdOverload(qb, QBCMD::onCMD_2A, "CMD", "aop1");
			CmdOverload(qb, QBCMD::onCMD_2S, "CMD", "note");
			CmdOverload(qb, QBCMD::onCMD_2I1, "CMD", "id");
			CmdOverload(qb, QBCMD::onCMD_2I2, "CMD", "page");
			MakeCommand("qb_test", "QuickBackupX Plugin Debug(Test) Command", 4);
			CmdOverload(qb_test, QBCMD::onCMD_Test);
		});
		ab->is_on = cfg->aoab;
		thread thab(&RunAutoBackup);
		thab.detach();
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		cout << "感谢 TISUnion(https://www.github.com/TISUnion/QuickBackupM) 的灵感支持" << endl;
		cout << "感谢 Bundle库(https://github.com/r-lyeh-archived/bundle) 的代码支持" << endl;
		cout << "感谢 LiteLoader(https://github.com/LiteLDev) 的代码支持" << endl;
		cout << "感谢 dreamguxiang(https://github.com/dreamguxiang) 的代码支持" << endl;
		cout << "感谢 JsonCPP库(https://github.com/open-source-parsers/jsoncpp) 的代码支持" << endl;
		cout << "感谢 OpenSSL库(https://github.com/openssl/openssl) 的代码支持" << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		cout << "插件已启动(Version " << QBXVERSION << " #" << QBXVERSIONTAG << "##BDXC Edition#)" << endl;
		cout << "Plugin GitHub Repository:https://www.github.com/Jasonzyt/QuickBackupX)" << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
	}

	void dll_exit()
	{
		L_INFO("析构全局对象...(你对象,无了/doge)");
		delete log;
		delete rec;
		delete cfg;
		delete ab;
	}
	
	SYMHOOK(GetXuid, Player*, "??0Player@@QEAA@AEAVLevel@@AEAVPacketSender@@W4GameType@@AEBVNetworkIdentifier@@EVUUID@mce@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$unique_ptr@VCertificate@@U?$default_delete@VCertificate@@@std@@@8@55@Z", 
		Player* _this, VA level, __int64 a3, int a4, __int64 a5, __int64 a6, void* xuid, 
		std::string& strxuid, __int64* a9, __int64 a10, __int64 a11) 
	{
		pxuid_level = level;
		return original(_this, level, a3, a4, a5, a6, xuid, strxuid, a9, a10, a11);
	}

	SYMHOOK(onServerCMDOutput, VA, "??$_Insert_string@DU?$char_traits@D@std@@_K@std@@YAAEAV?$basic_ostream@DU?$char_traits@D@std@@@0@AEAV10@QEBD_K@Z", 
		VA handle, char* str, VA size)
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
		string pname = p->getNameTag();
		string pxuid = p->getXuid(pxuid_level);
		OnlineQuan++;
		PlayerOnline[p] = true;
		PlayerIsOnline[pname] = true;
		PlayerUuid[p->getUuid()->toString()] = p;
		PlayerXuid[p->getNameTag()] = p->getXuid(pxuid_level);
		for (auto& it : cfg->admins)
		{
			if (it.second.empty() && it.second == pxuid)
			{
				cfg->EditPermissionXuid(Config::PerType::Admin, pname, pxuid);
			}
		}
		for (auto& it : cfg->backup)
		{
			if (it.second.empty() && it.second == pxuid)
			{
				cfg->EditPermissionXuid(Config::PerType::Backup, pname, pxuid);
			}
		}
		for (auto& it : cfg->back)
		{
			if (it.second.empty() && it.second == pxuid)
			{
				cfg->EditPermissionXuid(Config::PerType::Back, pname, pxuid);
			}
		}
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