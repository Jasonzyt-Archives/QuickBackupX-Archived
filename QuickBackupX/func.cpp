// Created by JasonZYT on 2021/02/05
#include "func.h"
#include "logger.h"
#include "config.h"
#include "properties.h"
#include "zip/ZipDefine.h"
#include "zip/zip.h"
#include "zip/unzip.h"
#include <iostream>
#include <sstream>
#pragma warning(disable:4996)

using namespace std;

namespace QuickBackupX
{
	string getTime()
	{
		time_t timep;
		time(&timep);
		char tmp[64];
		strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
		return tmp;
	}
	string getCustomTime(const char* format)
	{
		time_t timep;
		time(&timep);
		char tmp[64];
		strftime(tmp, sizeof(tmp), format, localtime(&timep));
		return tmp;
	}

	size_t getFileSize(std::string fn)
	{
		filesystem::path file(fn);
		if (filesystem::exists(file))
			return filesystem::file_size(file);
		return -1;
	}
	string getLevelName()
	{
		::Config pro("./server.properties");
		string rv = "Bedrock level";
		rv = pro.Read("level-name", rv);
		return rv;
	}

	wchar_t* CharToWChar(const char* str)
	{
		wchar_t* buffer = NULL;
		if (str)
		{
			size_t nu = strlen(str);
			size_t n = (size_t)MultiByteToWideChar(ZIP_UNICODE, 0, (const char*)str, int(nu), NULL, 0);
			buffer = 0;
			buffer = new wchar_t[n + 1];
			MultiByteToWideChar(ZIP_UNICODE, 0, (const char*)str, int(nu), buffer, int(n));
			buffer[n] = 0;
		}
		return buffer;
	}
	wstring WStringToString(string str)
	{
		// filesystem
		return filesystem::path(str).wstring();
	}
	string ToUTF8(string str)
	{
		return filesystem::path(str).u8string();
	}
	/// <summary>
	/// ���ַ�����һ���ַ��ֶ�
	/// </summary>
	/// <param name="s">��Ҫ�ֶε��ַ���</param>
	/// <param name="flag">�ֶε��ַ�</param>
	/// <returns>std::vector �ֶεĽ��</returns>
	vector<string> split(string s, const char flag) 
	{
		vector<string> rv;
		if (s != "")
		{	
			istringstream iss(s);
			string temp;
			while (getline(iss, temp, flag)) {
				rv.push_back(temp);
			}
		}
		return rv;
	}
	float round(float x, int pre)
	{
		ostringstream ostr;
		ostr << setiosflags(std::ios::fixed) << setprecision(pre);
		ostr << x;
		return atof(ostr.str().c_str());
	}
	double round(double x, int pre)
	{
		ostringstream ostr;
		ostr << setiosflags(std::ios::fixed) << setprecision(pre);
		ostr << x;
		return atof(ostr.str().c_str());
	}
	string round_str(double x, int pre)
	{
		ostringstream ostr;
		ostr << setiosflags(std::ios::fixed) << setprecision(pre);
		ostr << x;
		return ostr.str();
	}
	string SizeToString(size_t size)
	{
		if (size >= 1024 && size < 1048576)
			return round_str(static_cast<double>(size / 1024.00), 2) + "KB";
		else if (size >= 1048576 && size < 1073741824)
			return round_str(static_cast<double>(size / 1048576.00), 2) + "MB";
		else if (size >= 1073741824 && size < 1099511627776)
			return round_str(static_cast<double>(size / 1073741824.00), 2) + "GB";
		else if (size >= 1099511627776 && size < 1125899906842624)
			return round_str(static_cast<double>(size / 1099511627776.00), 2) + "TB";
		else
			return to_string(size) + "Byte";
	}
	string to_UTF8(const string& str)
	{
		int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
		wchar_t* pwBuf = new wchar_t[nwLen + 1];
		ZeroMemory(pwBuf, nwLen * 2 + 2);
		::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);
		int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
		char* pBuf = new char[nLen + 1];
		ZeroMemory(pBuf, nLen + 1);
		::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
		std::string retStr(pBuf);
		delete[]pwBuf;
		delete[]pBuf;
		pwBuf = NULL;
		pBuf = NULL;
		return retStr;
	}

	string ZipRetCheck(int in)
	{
		switch (in)
		{
		case ZR_OK:
			return "ѹ���ɹ�";
			break;
		case ZR_NOFILE:
			return "�Ҳ�����Ҫѹ�����ļ�";
			break;
		case ZR_NODUPH:
			return "�޷����ƾ��";
			break;
		case ZR_NOALLOC:
			return "δ�ܷ���ĳЩ��Դ";
			break;
		case ZR_WRITE:
			return "д���ļ�ʱ���ִ���";
			break;
		case ZR_NOTFOUND:
			return "��ѹ���ļ����Ҳ������ļ�";
			break;
		case ZR_MORE:
			return "���и�������Ҫ��ѹ��";
			break;
		case ZR_CORRUPT:
			return "ѹ���ļ����𻵻���ZIPѹ���ļ�";
			break;
		case ZR_READ:
			return "��ȡ�ļ�ʱ���ֳ������";
			break;
		case ZR_PASSWORD:
			return "û�л����ȷ����������ѹ���ļ�";
			break;
		case ZR_ARGS:
			return "��������";
			break;
		case ZR_NOTMMAP:
			return "����ZipGetMemory ����ֻ������MMap ZIP�ļ�";
			break;
		case ZR_MEMSIZE:
			return "�ڴ治��";
			break;
		case ZR_FAILED:
			return "���ô˺���ʱ�ò�����ʧ��";
			break;
		case ZR_ENDED:
			return "ѹ���ļ������ѹر�";
			break;
		case ZR_MISSIZE:
			return "��ʾ�������ļ���С����";
			break;
		case ZR_PARTIALUNZ:
			return "�ļ��Ѿ����ֽ�ѹ��";
			break;
		case ZR_ZMODE:
			return "���Ի�ϴ���/��ѹ���ļ�";
			break;
		default:
			return "δ����ı�ʶ��";
			break;
		}
	}

	Json::Value getJSON(std::string fn)
	{
		std::ifstream in;
		in.open(fn);
		std::string content((std::istreambuf_iterator<char>(in)), (std::istreambuf_iterator<char>()));
		in.close();
		if (content == "") { content = "{}"; }
		Json::Reader reader;
		Json::Value root;
		if (reader.parse(content, root))
		{
			return root;
		}
		return root;
	}
	bool SWriteIntoFile(Json::Value root, std::string fn)
	{
		Json::StyledWriter sw;
		ofstream fp;
		fp.open(fn, ios::out);
		if (!fp.is_open()) return false;
		fp << sw.write(root);
		fp.close();
		return true;
	}

	bool Is_Admin(string pname, string pxuid)
	{
		map<string, string>::iterator iter = cfg->admins.begin();
		for (; iter != cfg->admins.end(); ++iter)
		{
			if (iter->first == pname)
			{
				if (iter->second != pxuid)
				{
					L_WARNING(string("��� ") + pname + " AdminȨ�޼��ʧ��: Xuid��ƥ��");
					return false;
				}
				else return true;
			}
			if (iter->second == pxuid)
			{
				if (iter->first != pname)
					cfg->EditPermissionName(Config::PerType::Admin, pxuid, pname);
				return true;
			}
		}
		L_WARNING(string("��� ") + pname + " AdminȨ�޼��ʧ��: ��Ȩ��");
	}
	bool TellAdmin(std::string msg)
	{
		map<string, string>::iterator iter = cfg->admins.begin();
		for (; iter != cfg->admins.end(); ++iter)
		{
			if (PlayerIsOnline[iter->first])
				sendText(iter->first, msg);
		}
		return true;
	}
}