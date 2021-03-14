// Created by JasonZYT on  2021/02/06
#include "logger.h"
#include "pch.h"
#include "config.h"
#include "func.h"

using namespace std;

namespace QuickBackupX
{
	Logger::~Logger()
	{
		this->fp << "============================  Plugin exited  ============================" << endl << endl;
		this->fp.close();
	}

	bool Logger::Open()
	{
		if (!this->status)
		{
			this->fp.open(LOGFILE, ios::app | ios::out);
			if (!this->fp.is_open()) {
				PRWARN(u8"日志文件打开失败 :(");
				return false;
			}
			else this->status = true;
		}
		return true;
	}
	bool Logger::Start()
	{
		if (this->status)
		{
			if (!this->fp.is_open())
			{
				this->status = false;
				return false;
			}
			this->fp << "============================ Plugin injected ============================" << endl
				<< "Version: QuickBackupX " << QBXVERSION << " " << QBXVERSIONTAG << endl
				<< "Build time: " << __DATE__ << "  " << __TIME__ << endl;
			return true;
		}
		return false;
	}
	bool Logger::LogInfo(string evt)
	{
		if (this->status)
		{
			if (!this->fp.is_open())
			{
				this->status = false;
				return false;
			}
			this->fp << "[" << getTime() << " INFO] " << evt << endl;
			return true;
		}
		return false;
	}
	bool Logger::LogWarning(string evt, string fun, int line, string file)
	{
		string fn = filesystem::path(file).filename().string();
		if (this->status)
		{
			if (!this->fp.is_open())
			{
				this->status = false;
				return false;
			}
			if (cfg->debug && fun != "" && line != -1 && file != "")
				this->fp << "[" << getTime() << " WARN] " << evt << " (" << fun << ")(" << fn << ":" << line << ")" << endl;
			else if (cfg->debug && fun != "" && file != "")
				this->fp << "[" << getTime() << " WARN] " << evt << " (" << fun << ")(" << fn << ")" << endl;
			else if (cfg->debug && fun != "")
				this->fp << "[" << getTime() << " WARN] " << evt << " (" << fun << ")" << endl;
			else
				this->fp << "[" << getTime() << " WARN] " << evt << endl;
			return true;
		}
		return false;
	}

	bool Logger::LogError(string evt, string fun, int line, string file)
	{
		string fn = filesystem::path(file).filename().string();
		if (this->status)
		{
			if (!this->fp.is_open())
			{
				this->status = false;
				return false;
			}
			if (cfg->debug && fun != "" && line != -1 && file != "")
				this->fp << "[" << getTime() << " ERROR] " << evt << " (" << fun << ")(" << fn << ":" << line << ")" << endl;
			else if (cfg->debug && fun != "" && file != "")
				this->fp << "[" << getTime() << " ERROR] " << evt << " (" << fun << ")(" << fn << ")" << endl;
			else if (cfg->debug && fun != "")
				this->fp << "[" << getTime() << " ERROR] " << evt << " (" << fun << ")" << endl;
			else
				this->fp << "[" << getTime() << " ERROR] " << evt << endl;
			return true;
		}
		return false;
	}
	bool Logger::LogDebugInfo(string evt, string fun, int line, string file)
	{
		string fn = filesystem::path(file).filename().string();
		if (this->status)
		{
			if (!this->fp.is_open())
			{
				this->status = false;
				return false;
			}
			if (cfg->debug && fun != "" && line != -1 && file != "")
				this->fp << "[" << getTime() << " DEBUG] " << evt << " (" << fun << ")(" << fn << ":" << line << ")" << endl;
			else if (cfg->debug && fun != "" && file != "")
				this->fp << "[" << getTime() << " DEBUG] " << evt << " (" << fun << ")(" << fn << ")" << endl;
			else if (cfg->debug && fun != "")
				this->fp << "[" << getTime() << " DEBUG] " << evt << " (" << fun << ")" << endl;
			else
				this->fp << "[" << getTime() << " DEBUG] " << evt << endl;
			return true;
		}
		return false;
	}
	bool Logger::Log(string tag, string tags, string evt, string fun, int line, string file)
	{
		string fn = filesystem::path(file).filename().string();
		if (this->status)
		{
			if (!this->fp.is_open())
			{
				this->status = false;
				return false;
			}
			if (cfg->debug && fun != "" && line != -1 && file != "")
				this->fp << "[" << getTime() << " " << tag << "]" << tags << " " << evt << " (" << fun << ")(" << fn << ":" << line << ")" << endl;
			else if (cfg->debug && fun != "" && file != "")
				this->fp << "[" << getTime() << " " << tag << "]" << tags << " " << evt << " (" << fun << ")(" << fn << ")" << endl;
			else if (cfg->debug && fun != "")
				this->fp << "[" << getTime() << " " << tag << "]" << tags << " " << evt << " (" << fun << ")" << endl;
			else
				this->fp << "[" << getTime() << " " << tag << "]" << tags << " " << evt << endl;
			return true;
		}
		return false;
	}
}
