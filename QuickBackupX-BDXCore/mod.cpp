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
				PR(u8"����QuickBackupX�ļ��гɹ�!");
				L_INFO("����QuickBackupX�ļ��гɹ�!");
			}
			else
			{
				PRERR(u8"����QuickBackupX�ļ���ʧ��!");
				PRERR(u8"���̼����˳�");
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
		L_INFO("����EULA.txt...");
		if (!eula.is_open())
		{
			L_ERROR(string("�޷������ļ�: ") + EULAFILE);
			Sleep(3000);
			throw 102;
			return;
		}
		eula << 
u8R"(QuickBackupX  EULA
Copyright (C)2020-2021 JasonZYT

*ʹ��QuickBackupX���(���¼�ơ��������)ǰ������Ҫ����config.json�е�"EULA": false����Ϊ"EULA": true��ʾ�����Ķ���ͬ��EULA(�����û����Э��)
�ϴθ�������: 2021��2��23��

1. �������� 
  1.1 ����Ⲣͬ�⣬��ʹ�ñ�����Ĺ�������ɼ�������κ���(�����������ڼ������Ӳ���ϵ���)����������߾����е����Ρ�
  1.2 ��ʹ���κα�������������(���ο��������ʹ���˱����Դ��������/���)�����µ��κμ������(�����������ڼ�����������ļ���)�ͼ���������ļ�(�����������������ȡ��˽����)й©��
2. ���˵��
  2.1 �������ѭGPLv3.0Э�鿪Դ���κ�ʹ���˱����ԭ����Դ����(�������ʹ�õĵ����������)�����/�����Ӧ����Դ����(���¼�ơ���Դ��)��ע��ԭ���ߡ�
  2.2 �������ʹ�õĵ������⣬������߾����˽����Э�飬���뱾Э���޹ء�
  2.3 �κ��ˡ���֯�����岻���ڲ��(Դ����)�м�����û�������к��ĳ���(�����������ڼ������������)�󷢲���
  2.4 �κ��ˡ���֯�����岻�ý��˲����Ϊӯ���ֶΣ��ڻ����������۱������
  2.5 �κ��ˡ���֯�����岻����δ�����ԭ����(JasonZYT)�����������ñ������
  2.6 �κ��ˡ���֯�����岻���ƽⱾ�����
  2.7 ���ԭ���߱����Ա����������Ȩ����
  2.8 �������Ȩ������Ϊ���ԭ���ߡ�
  2.9 �ƽ⡢���δ������������ɵ�һ�к�����ƽ��ߡ����δ����߳е���
)";
		eula.close();
		if (!filesystem::exists(BACKUPRECFILE))
		{
			PR(u8"δ�ҵ����ݼ�¼Json: " << BACKUPRECFILE);
			ofstream fp;
			fp.open(BACKUPRECFILE, ios::app | ios::out);
			PR(u8"�����Դ������ݼ�¼Json...");
			if (!fp.is_open())
			{
				PRERR(u8"�޷������ļ�: " << BACKUPRECFILE << u8" �볢���ֶ�����");
				L_ERROR(string("�޷������ļ�: ") + BACKUPRECFILE);
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
				L_INFO(string("���� ") + cfg->getBackupDir() + " �ļ��гɹ�!");
			}
			else
			{
				PRERR(u8"���� " << cfg->getBackupDir() << " �ļ���ʧ��! �볢���ֶ�����");
				L_INFO(string("���� ") + cfg->getBackupDir() + " �ļ���ʧ��!!!");
				Sleep(3000);
				throw 100;
			}
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		PR(u8"��л TISUnion(https://www.github.com/TISUnion/QuickBackupM) �����֧��");
		PR(u8"��л ZipUtils��(https://www.codeproject.com/Articles/7530/Zip-Utils-Clean-Elegant-Simple-Cplusplus-Win) �Ĵ���֧��");
		PR(u8"��л JsonCPP��(https://github.com/open-source-parsers/jsoncpp) �Ĵ���֧��");
		PR(u8"��л OpenSSL��(https://github.com/openssl/openssl) �Ĵ���֧��");
		PR(u8"���������(Version " << QBXVERSION << u8" #" << QBXVERSIONTAG << u8"##BDXC&BDX Edition#)(GitHub Repository:https://www.github.com/ST-SKYTown/QuickBackupX)");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
	}

	void dll_exit()
	{
		L_INFO("����ȫ�ֶ���...(�����,����/doge)");
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
					If_Console{ PRERR(u8"����� " << params[2] << u8" ������ " << cmd); }
					If_Player{ sendText(exer.pname,string("��c����� ") + params[2] + " ������ " + cmd); }
					L_ERROR(string("- ����� ") + params[2] + " ������ " + cmd);
					rv.type = QBCMDT::ERRPAR2;
				}
			}
			else if (params[1] == "list")
			{
				if (paramsize == 2) rv.type = QBCMDT::List;
				else if (paramsize > 3)
				{
					If_Console{ PRERR(u8"����� " << params[4] << u8" ������ " << cmd); }
					If_Player{ sendText(exer.pname,string("��c����� ") + params[4] + " ������ " + cmd); }
					L_ERROR(string("- ����� ") + params[4] + " ������ " + cmd);
					rv.type = QBCMDT::ERRPAR3;
				}
				else if (rv.ParamType(2) == QBCMDParam::_int) rv.type = QBCMDT::List;
				else { 
					If_Console{ PRERR(u8"���� [page: int](ҳ��) ���Ϸ�!"); }
					If_Player{ sendText(exer.pname, "��c���� [page: int](ҳ��) ���Ϸ�!"); }
					L_ERROR("���� [page: int](ҳ��) ���Ϸ�!");
					rv.type = QBCMDT::ERRPAR2; 
				}
			}
			else if (params[1] == "del")
			{
				if (paramsize == 2)
				{
					If_Console{ PRERR(u8"���� [onum: int](���) ���Ϸ�!"); }
					If_Player{ sendText(exer.pname, "��c���� [onum: int](���) ���Ϸ�!"); }
					L_ERROR("���� [onum: int](���) ���Ϸ�!");
					rv.type = QBCMDT::ERRPAR2;
				}
				else if (paramsize == 3 && rv.ParamType(2) == QBCMDParam::_int) rv.type = QBCMDT::Del;
				else if (paramsize == 3 && params[2] == "all") rv.type = QBCMDT::Del;
				else if (paramsize == 3)
				{
					If_Console{ PRERR(u8"���� [onum: int](���) ���Ϸ�!"); }
					If_Player{ sendText(exer.pname, "��c���� [onum: int](���) ���Ϸ�!"); }
					L_ERROR("���� [onum: int](���) ���Ϸ�!");
					rv.type = QBCMDT::ERRPAR2;
				}
				else
				{
					If_Console{ PRERR(u8"����� " << params[4] << u8" ������ " << cmd); }
					If_Player{ sendText(exer.pname,string("��c����� ") + params[4] + " ������ " + cmd); }
					L_ERROR(string("- ����� ") + params[4] + " ������ " + cmd);
					rv.type = QBCMDT::ERRPAR3;
				}
			}
			else if (params[1] == "reload")
			{
				if (paramsize == 2) rv.type = QBCMDT::Reload;
				else
				{
					If_Console{ PRERR(u8"����� " << params[2] << u8" ������ " << cmd); }
					If_Player{ sendText(exer.pname,string("��c����� ") + params[2] + " ������ " + cmd); }
					L_ERROR(string("- ����� ") + params[2] + " ������ " + cmd);
					rv.type = QBCMDT::ERRPAR2;
				}
			}
			else if (params[1] == "help")
			{
				if (paramsize == 2) rv.type = QBCMDT::Help;
				else
				{
					If_Console{ PRERR(u8"����� " << params[2] << u8" ������ " << cmd); }
					If_Player{ sendText(exer.pname,string("��c����� ") + params[2] + " ������ " + cmd); }
					L_ERROR(string("- ����� ") + params[2] + " ������ " + cmd);
					rv.type = QBCMDT::ERRPAR2;
				}
			}
			else if (params[1] == "back")
			{
				if (paramsize == 2)
				{
					If_Console{ PRERR(u8"���� [onum: int](���) ���Ϸ�!"); }
					If_Player{ sendText(exer.pname, "��c���� [onum: int](���) ���Ϸ�!"); }
					L_ERROR("���� [onum: int](���) ���Ϸ�!");
					rv.type = QBCMDT::ERRPAR2;
				}
				else if (paramsize == 3)
				{
					if (rv.ParamType(2) != QBCMDParam::_int)
					{
						If_Console{ PRERR(u8"���� [onum: int](���) ���Ϸ�!"); }
						If_Player{ sendText(exer.pname, "��c���� [onum: int](���) ���Ϸ�!"); }
						L_ERROR("���� [onum: int](���) ���Ϸ�!");
						rv.type = QBCMDT::ERRPAR3;
					}
					else QBCMDT::Back;
				}
				else if (paramsize == 4)
				{
					if (rv.ParamType(2) != QBCMDParam::_int)
					{
						If_Console{ PRERR(u8"���� [onum: int](���) ���Ϸ�!"); }
						If_Player{ sendText(exer.pname, "��c���� [onum: int](���) ���Ϸ�!"); }
						L_ERROR("���� [onum: int](���) ���Ϸ�!");
						rv.type = QBCMDT::ERRPAR3;
					}
					else if (rv.ParamType(3) != QBCMDParam::_bool)
					{
						If_Console{ PRERR(u8"���� [delsrc: bool](�Ƿ�ɾ��Դ) ���Ϸ�!"); }
						If_Player{ sendText(exer.pname, "��c���� [delsrc: bool](�Ƿ�ɾ��Դ) ���Ϸ�!"); }
						L_ERROR("���� [delsrc: bool](�Ƿ�ɾ��Դ) ���Ϸ�!");
						rv.type = QBCMDT::ERRPAR4;
					}
					else QBCMDT::Back;
				}
				else if (paramsize == 5)
				{
					if (rv.ParamType(2) != QBCMDParam::_int)
					{
						If_Console{ PRERR(u8"���� [onum: int](���) ���Ϸ�!"); }
						If_Player{ sendText(exer.pname, "��c���� [onum: int](���) ���Ϸ�!"); }
						L_ERROR("���� [onum: int](���) ���Ϸ�!");
						rv.type = QBCMDT::ERRPAR3;
					}
					else if (rv.ParamType(3) != QBCMDParam::_bool)
					{
						If_Console{ PRERR(u8"���� [delsrc: bool](�Ƿ�ɾ��Դ) ���Ϸ�!"); }
						If_Player{ sendText(exer.pname, "��c���� [delsrc: bool](�Ƿ�ɾ��Դ) ���Ϸ�!"); }
						L_ERROR("���� [delsrc: bool](�Ƿ�ɾ��Դ) ���Ϸ�!");
						rv.type = QBCMDT::ERRPAR4;
					}
					else if (rv.ParamType(4) != QBCMDParam::_bool) 
					{
						If_Console{ PRERR(u8"���� [delevel: bool](�Ƿ�ɾ���浵) ���Ϸ�!"); }
						If_Player{ sendText(exer.pname, "��c���� [delevel: bool](�Ƿ�ɾ���浵) ���Ϸ�!"); }
						L_ERROR("���� [delevel: bool](�Ƿ�ɾ���浵) ���Ϸ�!");
						rv.type = QBCMDT::ERRPAR5;
					}
					else QBCMDT::Back;
				}
				else
				{
					If_Console{ PRERR(u8"����� " << params[6] << u8" ������ " << cmd); }
					If_Player{ sendText(exer.pname,string("��c����� ") + params[6] + " ������ " + cmd); }
					L_ERROR(string("- ����� ") + params[6] + " ������ " + cmd);
					rv.type = QBCMDT::ERRPAR6;
				}
			}
			else
			{
				If_Console{ PRERR(u8"���� <cmd: QBCMDT>(����) ���Ϸ�!"); }
				If_Player{ sendText(exer.pname, "��c���� <cmd: QBCMDT>(����) ���Ϸ�!"); }
				L_ERROR("���� <cmd: int>(����) ���Ϸ�!");
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
			L_INFO(string("����ִ̨����QB����: ") + *cmd);
			if (qcmd.type == QBCMDT::Make)
			{
				// Backup Main
				Backup* bak = new Backup;
				thread mkbackup(&Backup::Make, bak, exer); 
				mkbackup.detach(); // "detach�Ժ����̻߳��Ϊ�¶��̣߳��߳�֮�佫�޷�ͨ��"
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
						cout << u8"- ����[" << iter + 1 << "] " << baklist[iter]->time << " " << SizeToString(baklist[iter]->size) << endl;
					}
					return false;
				}
				int lpages = cfg->lops;
				double v1 = (baklist.size() / static_cast<double>(lpages));
				int page_quan = v1 + 0.99999999;
				if (pag > page_quan) pag = page_quan;
				if (page_quan == 0)
				{
					PRWARN(u8"�ޱ���!!! ���� \"qb make\" ����һ������");
					return false;
				}
				int startline = lpages * (pag - 1);
				int endline = lpages * pag - 1;
				cout << u8"========================= �����б� �� " << pag << "/" << page_quan << u8" ҳ �� " << baklist.size() << u8" ������ =========================" << endl;
				for (int iter = startline; iter <= endline && iter <= baklist.size() - 1; iter++)
				{
					cout << u8"- ����[" << iter + 1 << "] " << baklist[iter]->time << " " << SizeToString(baklist[iter]->size) << endl;
				}
				cout << u8"========================= �����б� �� " << pag << "/" << page_quan << u8" ҳ �� " << baklist.size() << u8" ������ =========================" << endl;
			}
			else if (qcmd.type == QBCMDT::Del)
			{
				if (qcmd.params[2] == "all")
				{
					PRWARN(u8"������ִ��ɾ��ȫ������!");
				}
				int onum = atoi(qcmd.params[2].c_str());
				if (onum >= rec->blist.size())
				{
					PRERR(u8"�Ҳ�������[" << onum << "]!����\"qb list\"�鿴��ǰ�ı���");
					L_ERROR("- ִ��ʧ��!");
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
					PR(u8"�������سɹ�!");
					L_INFO("�������سɹ�!");
				}
				else
				{
					PR(u8"��������ʧ��: " << CONFIGFILE << "�޷���!!!");
					L_INFO("��������ʧ��!");
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
			L_INFO(string("��� ") + pp->getNameTag() + " ִ����QB����: " + cmd);
			if (qcmd.type == QBCMDT::Make)
			{
				// Backup Main
				Backup* bak = new Backup;
				thread mkbackup(&Backup::Make, bak, exer);
				mkbackup.detach(); // "detach�Ժ����̻߳��Ϊ�¶��̣߳��߳�֮�佫�޷�ͨ��"
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
						sendText(pp->getNameTag(), "���� [page: int](ҳ��) ���Ϸ�,������!");
						L_ERROR("- ���� [page: int](ҳ��) ���Ϸ���");
						L_ERROR("- ִ��ʧ��!");
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
					PRWARN(u8"�ޱ���!!! ���� \"qb make\" ����һ������");
					return;
				}
				int startline = 30 * (pag - 1);
				int endline = 30 * pag - 1;
				cout << u8"========================= �����б� �� " << pag << "/" << page_quan << u8" ҳ �� " << baklist.size() << u8" ������ =========================" << endl;
				for (int iter = startline; iter <= endline && iter <= baklist.size() - 1; iter++)
				{
					cout << u8"- ����[" << iter + 1 << "] " << baklist[iter]->time << " " << SizeToString(baklist[iter]->size) << endl;
				}
				cout << u8"========================= �����б� �� " << pag << "/" << page_quan << u8" ҳ �� " << baklist.size() << u8" ������ =========================" << endl;
			}
			else if (qcmd.type == QBCMDT::Del)
			{
				if (qcmd.ParamType(2) != QBCMDParam::_int)
				{
					PRERR(u8"���� [onum: int](�������) ���Ϸ�,������!");
					L_ERROR("- ���� [onum: int](�������) ���Ϸ���");
					L_ERROR("- ִ��ʧ��!");
					return;
				}
				int onum = atoi(qcmd.params[2].c_str());
				rec->blist[(onum - 1)]->Delete(exer);
			}
			else
			{
				PRERR(u8"�������Ϸ�!");
				L_ERROR("- �������Ϸ���");
				L_ERROR("- ִ��ʧ��!");
				return;
			}
			return;
		}
		original(_this, id, crp);
	}*/

	SYMHOOK(onCBCMD, bool, "?performCommand@CommandBlockActor@@QEAA_NAEAVBlockSource@@@Z",
		VA _this, BlockSource* a2) {
		//����:0,�ظ�:1,��:2
		int mode = SYMCALL<int>("?getMode@CommandBlockActor@@QEBA?AW4CommandBlockMode@@AEAVBlockSource@@@Z",
			_this, a2);
		//������:0,������:1
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
		exer.cbpos = to_string(dim) + ":" + bp.toString(); // 1:0,0,0 ����0,0,0��
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