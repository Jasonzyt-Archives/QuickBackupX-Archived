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
	/// 将字符串以一个字符分段
	/// </summary>
	/// <param name="s">需要分段的字符串</param>
	/// <param name="flag">分段的字符</param>
	/// <returns>std::vector 分段的结果</returns>
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
			return "压缩成功";
			break;
		case ZR_NOFILE:
			return "找不到需要压缩的文件";
			break;
		case ZR_NODUPH:
			return "无法复制句柄";
			break;
		case ZR_NOALLOC:
			return "未能分配某些资源";
			break;
		case ZR_WRITE:
			return "写入文件时出现错误";
			break;
		case ZR_NOTFOUND:
			return "在压缩文件中找不到该文件";
			break;
		case ZR_MORE:
			return "还有更多数据要解压缩";
			break;
		case ZR_CORRUPT:
			return "压缩文件已损坏或不是ZIP压缩文件";
			break;
		case ZR_READ:
			return "读取文件时出现常规错误";
			break;
		case ZR_PASSWORD:
			return "没有获得正确的密码来解压缩文件";
			break;
		case ZR_ARGS:
			return "参数错误";
			break;
		case ZR_NOTMMAP:
			return "尝试ZipGetMemory 但这只适用于MMap ZIP文件";
			break;
		case ZR_MEMSIZE:
			return "内存不足";
			break;
		case ZR_FAILED:
			return "调用此函数时该操作已失败";
			break;
		case ZR_ENDED:
			return "压缩文件创建已关闭";
			break;
		case ZR_MISSIZE:
			return "显示的输入文件大小错误";
			break;
		case ZR_PARTIALUNZ:
			return "文件已经部分解压缩";
			break;
		case ZR_ZMODE:
			return "尝试混合创建/打开压缩文件";
			break;
		default:
			return "未定义的标识符";
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
					L_WARNING(string("玩家 ") + pname + " Admin权限检查失败: Xuid不匹配");
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
		L_WARNING(string("玩家 ") + pname + " Admin权限检查失败: 无权限");
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