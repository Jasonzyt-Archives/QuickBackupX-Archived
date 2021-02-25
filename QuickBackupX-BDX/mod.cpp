/* Created by JasonZYT on 2021/02/05
#include "pch.h"
#include "logger.h"
#include "config.h"
#include "json.h"
#include "func.h"
#include "backup.h"
#include "json/json.h"
#include "bds.hpp"
#include "bdx/mcapi/CommandReg.h"
#include "bdx/api/command/commands.h"
#include "bdx/mcapi/mass.h"
#pragma warning(disable:4996)
#define Is_Console (ori.getOriginType() == OriginType::DedicatedServer)
#define Is_Player  (ori.getOriginType() == OriginType::Player)
#define Is_CB      (ori.getOriginType() == OriginType::Block)
#define Is_MCB     (ori.getOriginType() == OriginType::MinecartBlock)

using namespace std;

namespace QuickBackupX
{
	bool Is_BDX()
	{
		HMODULE s = LoadLibrary(L"lightbase.dll");
		if (s == nullptr)
			return false;
		return true;
	}
	bool is_bdx = Is_BDX();

	BRecJson* rec = new BRecJson;
	Logger*   log = new Logger;
	Config*   cfg = new Config;

	// CMD Enums
	enum class QBCMD_Enum_Make : int
	{
		make = 1
	};
	enum class QBCMD_Enum_List : int
	{
		list = 2
	};
	enum class QBCMD_Enum_Del  : int
	{
		del = 3
	};
	enum class QBCMD_Enum_Back : int
	{
		back = 4
	};
	enum class QBCMD_Enum_Help : int
	{
		help = 5
	};
	enum class QBCMD_Enum_Menu : int
	{
		menu = 6
	};
	enum class QBCMD_Enum_View : int
	{
		view = 7
	};
	enum class QBCMD_Enum_Reload : int
	{
		reload = 8
	};
	// CMD Functions
	bool QBCMD_Make(CommandOrigin const& ori, CommandOutput& outp, MyEnum<QBCMD_Enum_Make>);
	bool QBCMD_List(CommandOrigin const& ori, CommandOutput& outp, MyEnum<QBCMD_Enum_List>, optional<int> page = 1);
	bool QBCMD_Del (CommandOrigin const& ori, CommandOutput& outp, MyEnum<QBCMD_Enum_Del >, optional<int> bnum);
	bool QBCMD_Back(CommandOrigin const& ori, CommandOutput& outp, MyEnum<QBCMD_Enum_Back>, optional<int> bnum);
	bool QBCMD_Help(CommandOrigin const& ori, CommandOutput& outp, MyEnum<QBCMD_Enum_Help>);
	bool QBCMD_Menu(CommandOrigin const& ori, CommandOutput& outp, MyEnum<QBCMD_Enum_Menu>);
	bool QBCMD_View(CommandOrigin const& ori, CommandOutput& outp, MyEnum<QBCMD_Enum_View>, optional<int> bnum);
	bool QBCMD_Reload(CommandOrigin const& ori, CommandOutput& outp, MyEnum<QBCMD_Enum_Reload>);

	void init()
	{
		PR(u8"QuickBackupX Loaded! Author: JasonZYT");
		if (!Exist(L"./QuickBackupX"))
		{
			PR(u8"首次启动本插件 正在初始化相关配置");
			int mkres = _wmkdir(L"QuickBackupX");
			if (mkres == 0)
			{
				PR(u8"创建QuickBackupX文件夹成功!");
				log->Open();
				log->Start();
				L_INFO("Create QuickBackupX folder succeed");
			}
			else
			{
				PRERR(u8"创建QuickBackupX文件夹失败!");
				PRERR(u8"进程即将退出");
				Sleep(3000);
				exit(100);
			}
			if (!filesystem::exists(CONFIGFILE))
			{
				string dcfgstr = R"({"Debug":true,"BackupOutputPath":"C:/backup/%Y-%m-%d %H:%M:%S.zip","License":"","Cloud_Backup_Player":[{"Name":"Steve","Xuid":"114514"},{"Name":"Alex"}],"Backup_Player":[],"Cloud_Back_Player":[],"Back_Player":[]})";
				Json::Value root;
				Json::Reader reader;
				Json::StyledWriter sw;
				reader.parse(dcfgstr, root);
				ofstream cfgf;
				cfgf.open(CONFIGFILE, ios::app | ios::out);
				if (!cfgf.is_open())
				{
					PRERR(u8"无法创建文件: " << CONFIGFILE << u8" 请尝试手动创建");
					log->LogError(string("无法创建文件: ") + CONFIGFILE);
					Sleep(3000);
					exit(102);
				}
				cfgf << sw.write(root);
				cfgf.close();
			}
			if (!filesystem::exists(BACKUPRECFILE))
			{
				PR(u8"未找到备份记录Json: " << BACKUPRECFILE);
				ofstream fp;
				fp.open(BACKUPRECFILE, ios::app | ios::out);
				PR(u8"正尝试创建备份记录Json...");
				if (!fp.is_open())
				{
					PRERR(u8"无法创建文件: " << BACKUPRECFILE << u8" 请尝试手动创建");
					log->LogError(string("无法创建文件: ") + BACKUPRECFILE);
					Sleep(3000);
					exit(102);
				}
				fp << "[]" << endl;
				fp.close();
			}
		}
		else
		{
			log->Open();
			log->Start();
		}
		if (cfg->getConfig())
		{
			PR(u8"配置读取成功!");
			L_INFO("Read config succeed");
		}
		else
		{
			PRERR(u8"配置读取失败!");
			log->LogError("Read config failed");
			PRERR(u8"进程即将退出");
			Sleep(10000);
			exit(101);
		}
		addListener([](RegisterCommandEvent& ev) {
			CEnum<QBCMD_Enum_Make> _ce1("qbmake", { "make" });
			CEnum<QBCMD_Enum_List> _ce2("qblist", { "list" });
			MakeCommand("qb", "QuickBackupX Command", CommandPermissionLevel::Normal);
			CmdOverload(qb, QBCMD_Make, "make");
			CmdOverload(qb, QBCMD_List, "list", "page");
			}
		);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		PR(u8"感谢 TISUnion(https://www.github.com/TISUnion/QuickBackupM) 的灵感支持");
		PR(u8"感谢 ZipUtils作者提供的ZIP库(https://www.codeproject.com/Articles/7530/Zip-Utils-Clean-Elegant-Simple-Cplusplus-Win)");
		PR(u8"插件已启动(Version " << QBXVERSION << u8" #" << QBXVERSIONTAG << u8"##BDXC&BDX Edition#)(GitHub Repository:https://www.github.com/ST-SKYTown/QuickBackupX)");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
	}

	bool QBCMD_Make(CommandOrigin const& ori, CommandOutput& outp, MyEnum<QBCMD_Enum_Make>)
	{
		Backup* bak = new Backup;
		if (ori.getOriginType() == OriginType::DedicatedServer)
		{
			bak->Make();
		}
		else if (ori.getOriginType() == OriginType::Player)
		{
			optional<WPlayer> wp = MakeWP(ori);
			string pname = wp.value().getName();
			string pxuid = to_string(wp.value().getXuid());
			bak->exer.pname = pname;
			bak->exer.pxuid = pxuid;
			bak->Make();
		}
		return true;
	}

	bool QBCMD_List(CommandOrigin const& ori, CommandOutput& outp, MyEnum<QBCMD_Enum_List>, optional<int> page)
	{
		int pag = 1;
		if (page.value() > 0) pag = page.value();
		vector<Backup*> baklist = rec->ListRecord();
		int page_quan = ceil(baklist.size() / 30);
		if (!page_quan)
		{
			if Is_Console
			{
				PRWARN(u8"无备份!!! 输入 \"qb make\" 创建一个备份");
				return true;
			}
			else if Is_Player
			{
				sendText(ori.getName(),"§e无备份!!! 输入 \"qb make\" 创建一个备份");
				return true;
			}
		}
		int startline = 30 * (pag - 1);
		int endline = 30 * pag - 1;
		if (ori.getOriginType() == OriginType::DedicatedServer)
		{
			L_INFO(string("服务端执行了命令: qb list ") + to_string(page.value()));
			cout << u8"========================= 备份列表 第 " << pag << "/" << page_quan << u8" 页 =========================" << endl;
		}
		else if (ori.getOriginType() == OriginType::Player)
		{
			L_INFO(string("玩家 ") + ori.getName() + " 执行了命令: qb list " + to_string(page.value()));
			sendText(ori.getName(), string("========================= 备份列表 第 ") + to_string(pag) + "/" + to_string(page_quan) + u8" 页 =========================");
		}
		else 
		{
			PRERR("不受支持的命令源类型: " << (int)ori.getOriginType() << " 试图执行命令: qb list " << page.value());
			L_WARNING(string("不受支持的命令源类型: ") + to_string((int)ori.getOriginType()) + " 试图执行命令: qb list " + to_string(page.value()));
			return false;
		}
		for (int iter = startline; iter <= endline && iter <= baklist.size(); iter++)
		{
			if (ori.getOriginType() == OriginType::DedicatedServer)
			{
				cout << u8"- 备份[" << iter + 1 << "] " << baklist[iter]->time << endl;
			}
			else if (ori.getOriginType() == OriginType::Player)
			{
				sendText("all", string("- 备份[") + to_string(iter + 1) + "] " + baklist[iter]->time);
			}
		}
	}

	SYMHOOK(onPlayerJoined, VA, "?onPlayerJoined@ServerScoreboard@@UEAAXAEBVPlayer@@@Z", VA a1, Player* p)
	{
		PlayerOnline[p] = true;
		PlayerUuid[p->getUuid()->toString()] = p;
		return original(a1, p);
	}

	SYMHOOK(onPlayerLeft, void, "?_onPlayerLeft@ServerNetworkHandler@@AEAAXPEAVServerPlayer@@_N@Z", VA _this, Player* p, char v3) 
	{
		PlayerOnline[p] = false;
		PlayerUuid.erase(p->getUuid()->toString());
		return original(_this, p, v3);
	}
}
*/

#include "config.h"
#include "logger.h"
#include "json.h"
#include "bds.hpp"
using namespace std;
namespace QuickBackupX
{
	Config* cfg = new Config;
	Logger* log = new Logger;
	BRecJson* rec = new BRecJson;
	std::map<Player*, bool> PlayerOnline;
	std::map<string, Player*> PlayerUuid;
	std::map<string, bool> PlayerIsOnline;
	std::map<unsigned, bool> fids;
	void dll_init(){}
	void dll_exit(){}
}