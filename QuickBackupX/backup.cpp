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
		}
	}

	string Backup::Executor::format_pos()
	{
		string rv;
		if (type != Block_Type)
		{
			L_ERROR("����Block����!!!");
			throw 104;
		}
		if (cbpos == "")
		{
			L_ERROR("cbposΪ��!!!");
			throw 105;
		}
		vector<string> pos = split(cbpos, ':');
		if (pos.size() != 2) throw 106;
		string dim = "δ֪ά��";
		switch (atoi(pos[0].c_str()))
		{
		case 0: dim = "������"; break;
		case 1: dim = "�½�"; break;
		case 2: dim = "ĩ��"; break;
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
		string bdisk = cfg->getBackupDisk();
		size_t lsize = this->getLevelSize();
		if (!this->CheckBackupPermission())
		{
			if (this->exer.type == Player_Type)
			{
				PRWARN(u8"��� " << this->exer.pname << u8" ��ͼִ�б���,������Ȩ�ޱ�������");
				L_WARNING(string("��� ") + this->exer.pname + "��ͼִ�б���(��Ȩ��)");
				TellAdmin(string("��� ") + this->exer.pname + "(��Ȩ��) ��ͼִ�б���");
				sendText(this->exer.pname, "��c��û��Ȩ��ִ�д˲���!!!");
			}
			else if (this->exer.type == Block_Type)
			{
				PRWARN(u8"�����(λ�� " << this->exer.cbpos << u8") ��ͼִ�б���,���������ļ�������������");
				L_WARNING(string("�����(λ�� ") + this->exer.cbpos + ") ��ͼִ�б���");
			}
			return false;
		}
		PR(u8"��������...");
		sendText("all", string("��b[QuickBackupX] ��ʼ��������: ") + this->time);
		ULARGE_INTEGER bavfree;
		ULARGE_INTEGER btotal;
		ULARGE_INTEGER bfree;
		GetDiskFreeSpaceExA(bdisk.c_str(), &bavfree, &btotal, &bfree);
		if (to_Byte(bavfree) <= lsize)
		{
			PRWARN(u8"���̿��ÿռ����ڵ�ǰ�浵��С,��ȡ������!");
			sendText("all", "��c���̿��ÿռ����ڵ�ǰ�浵��С,��ȡ������!");
			return false;
		}
		LARGE_INTEGER freq;
		LARGE_INTEGER begin;
		LARGE_INTEGER end;
		double timec;
		Debug{ L_INFO("���ݿ�ʼ��ʱ"); }
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&begin);
		ZRESULT res = this->Create(); // ִ�б���
		QueryPerformanceCounter(&end);
		timec = (double)(end.QuadPart - begin.QuadPart) / (double)freq.QuadPart; // ��ȡ��ʱ���

		if (res == ZR_OK)
		{
			Debug{ L_INFO("��ʼ��ȡ������Ϣ"); }
			if (!filesystem::exists(this->path))
			{
				PRERR(u8"�����ļ�������");
				PRERR(u8"����ʧ��!!!");
				sendText("all", "��c����ʧ��!");
				return false;
			}
			this->size = getFileSize(this->path.string());
			FILE* fp = fopen(this->path.string().c_str(), "rb");
			this->md5 = md5file(fp);
			fclose(fp);
			rec->AddRecord(this);
			int quan = rec->getBackupQuantity();
			string ssize = SizeToString(this->size);
			string disk = this->path.string().substr(0, 3);

			ULARGE_INTEGER avfree;
			ULARGE_INTEGER total;
			ULARGE_INTEGER free;
			GetDiskFreeSpaceExA(disk.c_str(), &avfree, &total, &free);
			if (to_GB(total) == 0)
			{
				L_ERROR("���㾯��!!!");
				PRERR(u8"���㾯��!!!");
				Sleep(3000);
				throw 103;
				return false;
			}
			float diskur = (to_GB(total) - to_GB(avfree)) / to_GB(total) * 100;
			float diskus = to_GB(total) - to_GB(free);
			double fcompr = ((double)this->size / lsize) * 100;
			string compr = round_str(fcompr, 2) + "%";

			L_INFO("���ݳɹ�(0)");
			PR(u8"�������! (��ʱ " << timec << u8" ��)");
			sendText("all", string("��a�������! (��ʱ ") + to_string(timec) + u8" ��)");
			PR(u8"- �˴α��ݴ浵��С: " << ssize << "(" << this->size << " B)");
			PR(u8"- �˴α��ݴ浵·��: " << this->path.string());
			PR(u8"- �˴α��ݴ浵MD5: " << this->md5);
			PR(u8"- �˴α��ݵ�ִ����: " << this->exer.to_string());
			PR(u8"- �˴α��ݵ�ѹ����: " << compr << u8" (ԭ�浵��С:" << SizeToString(lsize) << ")");
			PR(u8"- ���ݺ��" << disk[0] << u8"��״̬: " << u8"���� " << diskus << u8"GB, ���� " << to_GB(avfree) << u8"GB, ʹ����: " << diskur << "%");
			PR(u8"��ǰ���� " << quan << u8" ������ ����\"qb list\"�Ի�ø��౸����Ϣ");
			sendText("all", string("- ��c�˴α��ݴ浵��С: ") + ssize + "(" + to_string(this->size) + " B)");
			sendText("all", string("- ��6�˴α��ݴ浵·��: ") + this->path.string());
			sendText("all", string("- ��e�˴α��ݴ浵MD5: ") + this->md5);
			sendText("all", string("- ��a�˴α��ݵ�ִ����: ") + this->exer.to_string());
			sendText("all", string("- ��3�˴α��ݵ�ѹ����: ") + compr + " (ԭ�浵��С:" + SizeToString(lsize) + ")");
			sendText("all", string("- ��b���ݺ��") + disk[0] + "��״̬: " + "���� " + to_string(diskus) + "GB, ���� " + to_string(to_GB(avfree)) + "GB, ʹ����: " + to_string(diskur) + "%");
			sendText("all", string("��5��ǰ���� ") + to_string(quan) + " ������ ����\"qb list\"�Ի�ø��౸����Ϣ");
			return true;
		}
		else
		{
			PR(u8"����ʧ��~ ([" << res << "]" << to_UTF8(ZipRetCheck(res)) << ")");
			sendText("all", "��c����ʧ��~ ����ϵ����");
			L_ERROR(string("����ʧ��: (") + to_string(res) + ")" + ZipRetCheck(res));
			return false;
		}
	}

	bool Backup::Delete(Backup::Executor exer)
	{
		L_INFO(string("ɾ������[") + to_string(this->onum) + "]");
		if (this->CheckDeletePermission(exer))
		{
			if (!filesystem::exists(this->path))
			{
				L_WARNING(string("- δ�ҵ�����[") + to_string(this->onum) + "] " + this->path.string() + " �ļ�");
				PRWARN(u8"δ�ҵ�����[" << this->onum << "] " << this->path.string() << u8" �ļ�");
				PR(u8"ɾ������[" << this->onum << u8"] �ļ�¼...");
				bool drres = rec->DeleteRecord(this->onum);
				if (exer.type == Player_Type)
				{
					sendText(exer.pname, string("��eδ�ҵ�����[") + to_string(this->onum) + "] " + this->path.string() + " �ļ�");
					TellAdmin(string("��eδ�ҵ�����[") + to_string(this->onum) + "] " + this->path.string() + " �ļ�");
					sendText(exer.pname, string("��c����[") + to_string(this->onum) + "] ��¼��ɾ��");
					TellAdmin(string("��c����[") + to_string(this->onum) + "] ��¼��ɾ��");
				}
				return true;
			}
			else
			{
				PR(u8"ɾ�������ļ�...");
				L_INFO("- ɾ�������ļ�...");
				bool fsres = filesystem::remove(this->path);
				PR(u8"ɾ�����ݼ�¼...");
				L_INFO("- ɾ�����ݼ�¼...");
				bool drres = rec->DeleteRecord(this->onum);
				if (!fsres)
				{
					PRERR(u8"ɾ�������ļ�ʱ�������쳣!!!");
					L_ERROR("- ɾ�������ļ�ʱ�������쳣!!!");
				}
				if (!drres)
				{
					PRERR(u8"ɾ�����ݼ�¼ʱ�������쳣!!!");
					L_ERROR("- ɾ�����ݼ�¼ʱ�������쳣!!!");
				}
				if (fsres && drres)
				{
					PR(u8"ɾ���ɹ�!");
					L_INFO("- ɾ���ɹ�!");
				}
				//delete this; MSVC��������,����ƽ̨���ܲ���������
			}
		}
		else
		{
			sendText(exer.pname, "��c��û��Ȩ��ִ�д˲���!!!");
			TellAdmin(string("��c") + exer.pname + " ��ͼִ��ɾ������!!!");
		}
		return false;
	}

	bool Backup::Resume(Backup::Executor exer)
	{
		PR(u8"��ʼ�ص�!�����Թر�BDS������ȡ���˻ص�(���ܵ��´���)");
		L_INFO("��ʼ�ص�!");
		DWORD pid = GetCurrentProcessId();
		if (!filesystem::exists(this->path))
		{
			PRWARN(u8"��Ҫ�ص��Ĵ浵�Ѳ�����!ɾ����¼...");
			L_WARNING("��Ҫ�ص��Ĵ浵�Ѳ�����!ɾ����¼...");
			if (exer.type == Player_Type)
			{
				sendText(exer.pname, "��Ҫ�ص��Ĵ浵�Ѳ�����!ɾ����¼...");
				TellAdmin("��Ҫ�ص��Ĵ浵�Ѳ�����!ɾ����¼...");
			}
			rec->DeleteRecord(this->onum);
			return false;
		}
		L_INFO("�򿪱���...");
		HZIP hz = OpenZip(this->path.wstring().c_str(), 0);
		ZIPENTRY ze; 
		GetZipItem(hz, -1, &ze); 
		int numitems = ze.index;
		L_INFO("��ʼ��ѹ...");
		for (int i = 0; i < numitems; i++)
		{
			GetZipItem(hz, i, &ze);
			Debug{ L_INFO(string("- ��ѹ ") + filesystem::path(ze.name).string() + " �� " + RESUMEDIR + filesystem::path(ze.name).string()); }
			UnzipItem(hz, i, CharToWChar((RESUMEDIR + filesystem::path(ze.name).string()).c_str()));
		}
		ZRESULT res = CloseZip(hz);
		if (res == ZR_OK)
		{
			L_INFO("��ѹ�ɹ�(0)");
			PR(u8"��ѹ�ɹ�!");
		}
		else
		{
			L_ERROR(string("��ѹʧ��: ([") + to_string(res) + "]" + ZipRetCheck(res) + ")");
			PR(u8"��ѹʧ��~ ([" << res << "]" << ZipRetCheck(res) << ")");
		}
		
	}

	bool Backup::CheckBackupPermission()
	{
		if (this->exer.type == Console_Type) return true;
		if (this->exer.type == Block_Type) return cfg->acb ? true : false;
		if (Is_Admin(this->exer.pname, this->exer.pxuid)) return true;
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
		string bpath = getCustomTime(cfg->bop.c_str());
		this->CopyLevelToTempDir();
		this->IterTempDir();
		size_t flsize = this->flist.size();
		Debug{ L_INFO(string("���� ") + bpath + " �ļ�..."); }
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
		this->hz = CreateZip(CharToWChar(bpath.c_str()), 0);
		if (this->hz == NULL) throw 107;
		L_INFO(string("��ʼѹ�� ���� ") + to_string(flsize) + " ���ļ���ѹ��");
		PR(u8"��ʼѹ�� ���� " << flsize << u8" ���ļ���ѹ��");
		sendText("all", string("��ʼѹ�� ���� ") + to_string(flsize) + " ���ļ���ѹ��");
		map<string, filesystem::path>::iterator iter = this->flist.begin();
		i = 0;
		for (; iter != this->flist.end(); ++iter)
		{
			i++;
			sendText("all", string("����ѹ���� ") + to_string(i) + " / " + to_string(flsize) + " ���ļ�");
			Debug{ L_INFO(string("- ����ļ�: ") + iter->second.string() + " �� " + iter->first); }
			ZipAdd(this->hz, CharToWChar(iter->first.c_str()), CharToWChar(iter->second.string().c_str()));
		}
		this->DeleteTempDir();
		Debug{ L_INFO(string("�رղ�д�� ") + bpath + " �ļ�..."); }
		ZRESULT rv = CloseZip(this->hz);
		if (filesystem::exists(bpath))
			this->path = filesystem::canonical(filesystem::path(bpath));
		return rv;
	}

	bool Backup::CopyLevelToTempDir()
	{
		if (exists(filesystem::path(TEMPDIR)))
		{
			PRWARN(u8"��ʱĿ¼(" << TEMPDIR << u8")�Ѿ�����!!!");
			L_WARNING("��ʱĿ¼(" + TEMPDIR + ")�Ѿ�����!!!");
			this->DeleteTempDir();
		}
		filesystem::copy(this->lpath, filesystem::path(TEMPDIR));
		Debug{ L_INFO(string("�Ѹ��� ").append(this->lpath.string().append(" �� ").append(TEMPDIR))); }
		L_INFO(string("��ʼ����Ŀ¼:").append(this->lpath.string()));
		Debug{ L_INFO("�ѹ��� std::filesystem::path ����"); }
		if (!exists(this->lpath))
		{
			PRERR(u8"Ŀ¼ " << this->lpath.string() << u8" ������!!!");
			L_ERROR(string("Ŀ¼ ") + this->lpath.string() + " ������!!!");
			return false;
		}
		if (!exists(filesystem::path(TEMPDIR)))
		{
			PRERR(u8"Ŀ¼ " << TEMPDIR << u8" ������!!!");
			L_ERROR(string("Ŀ¼ ") + TEMPDIR + u8" ������!!!");
			return false;
		}
		filesystem::directory_entry entry(this->lpath);
		Debug{ L_INFO("�ѹ��� std::filesystem::directory_entry ����"); }
		if (entry.status().type() != filesystem::file_type::directory)
		{
			PRERR(this->lpath.string() << u8" �����ļ���!!!");
			L_ERROR(this->lpath.string() + " �����ļ���!!!");
			return false;
		}
		filesystem::directory_iterator list(this->lpath);
		Debug{ L_INFO("�ѹ��� std::filesystem::directory_iterator ����"); }
		for (auto& it : list)
		{
			if (it.status().type() == filesystem::file_type::directory)
			{
				Debug{ L_INFO(string("- �ļ���: ").append(it.path().string())); }
				filesystem::copy(it.path(), filesystem::path(TEMPDIR + it.path().filename().string()));
				Debug{ L_INFO(string("�Ѹ��� ") + it.path().string() + " �� " + TEMPDIR + it.path().filename().string()); }
			}
		}
		return true;
	}

	bool Backup::DeleteTempDir()
	{
		Debug{ L_INFO(string("��ʼɾ����ʱĿ¼: ") + TEMPDIR); }
		if (!exists(filesystem::path(TEMPDIR)))
		{
			PRWARN(u8"��ʱĿ¼(" << TEMPDIR << u8")������!!!");
			L_WARNING("��ʱĿ¼(" + TEMPDIR + ")������!!!");
			return true;
		}
		size_t dfc = filesystem::remove_all(filesystem::path(TEMPDIR));
		Debug{ L_INFO(TEMPDIR + " ��ɾ��,��ɾ���� " + to_string(dfc) + " ���ļ�"); }
		return true;
	}

	size_t Backup::getLevelSize()
	{
		return filesystem::file_size(this->lpath);
	}

	bool Backup::IterTempDir(string path)
	{
		string tempd;
		if (path == "") tempd = TEMPDIR;
		else tempd = TEMPDIR + path + "/";
		filesystem::path dir(tempd);
		L_INFO(string("��ʼ����Ŀ¼:").append(tempd));
		Debug{ L_INFO("�ѹ��� std::filesystem::path ����"); }
		if (!exists(dir))
		{
			PRERR(u8"Ŀ¼ " << tempd << u8" ������!!!");
			L_ERROR(string("Ŀ¼ ") + tempd + " ������!!!");
			return false;
		}
		filesystem::directory_entry entry(dir);
		Debug{ L_INFO("�ѹ��� std::filesystem::directory_entry ����"); }
		if (entry.status().type() != filesystem::file_type::directory)
		{
			PRERR(tempd << u8" �����ļ���!!!");
			L_ERROR(tempd + " �����ļ���!!!");
			return false;
		}
		filesystem::directory_iterator list(dir);
		Debug{ L_INFO("�ѹ��� std::filesystem::directory_iterator ����"); }
		for (auto& it : list)
		{
			if (it.path().filename().string() == "lost") continue;
			if (it.status().type() == filesystem::file_type::directory)
			{
				string in = path + it.path().filename().string();
				IterTempDir(in);
				continue;
			}
			if (path == "")
			{
				this->flist.insert(
					pair<string, filesystem::path>(it.path().filename().string(), it.path())
				);
			}
			else
			{
				this->flist.insert(
					pair<string, filesystem::path>(path + "/" + it.path().filename().string(), it.path())
				);
			}
			Debug{ L_INFO(string("- �ļ�: ").append(it.path().string())); }
		}
		return true;
	}
}