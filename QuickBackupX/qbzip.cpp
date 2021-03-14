// Created by JasonZYT on 2021/03/08
#include "qbzip.h"

using namespace std;

namespace QuickBackupX
{
	QBZIP::QBZIP(string zfp)
	{
		this->zfps = zfp;
		filesystem::path path(zfp);
		this->zfp = path;
		try {
			if (filesystem::status(path).type() != filesystem::file_type::regular)
			{
				if (filesystem::status(path).type() == filesystem::file_type::directory)
				{
					PRERR(path << " 不是文件!");
					L_ERROR(path.string() + " 不是文件!");
					throw path.string() + " 不是文件!";
				}
				if (!filesystem::exists(path))
				{
					if (!filesystem::exists(path.remove_filename()))
						filesystem::create_directories(path.remove_filename());
					this->file.open(path.string(), ios::out | ios::app);
					this->file.close();
				}
			}
		}
		catch (filesystem::filesystem_error fe) {
			PRERR("FILESYSTEM-ERR: " << to_UTF8(fe.what()));
			L_ERROR(to_UTF8(fe.what()));
			L_ERROR(string("- Path1: ") + fe.path1().string());
			L_ERROR(string("- Path2: ") + fe.path2().string());
		}
		catch (...) {
			L_ERROR("出现错误!!!");
		}
	}
	QBZIP::QBZIP(filesystem::path zfp)
	{
		this->zfps = zfp.string();
		this->zfp = zfp;
		try {
			if (filesystem::status(zfp).type() != filesystem::file_type::regular)
			{
				if (filesystem::status(zfp).type() == filesystem::file_type::directory)
				{
					PRERR(zfp << " 不是文件!");
					L_ERROR(zfp.string() + " 不是文件!");
					throw zfp.string() + " 不是文件!";
				}
				if (!filesystem::exists(zfp))
				{
					if (!filesystem::exists(zfp.remove_filename()))
						filesystem::create_directories(zfp.remove_filename());
					this->file.open(zfp.string(), ios::out | ios::app);
					this->file.close();
				}
			}
		}
		catch (filesystem::filesystem_error fe) {
			PRERR("FILESYSTEM-ERR: " << to_UTF8(fe.what()));
			L_ERROR(to_UTF8(fe.what()));
			L_ERROR(string("- Path1: ") + fe.path1().string());
			L_ERROR(string("- Path2: ") + fe.path2().string());
		}
		catch (...) {
			L_ERROR("出现错误!!!");
		}
	}
	QBZIP::~QBZIP()
	{
		this->file.close();
	}

	bool QBZIP::Add(string fn, string zp)
	{
		lock_guard<std::mutex> alock(this->mutex);
		size_t sz = this->arch.size();
		this->arch.resize(sz + 1);
		this->arch[sz]["name"] = zp;
		fstream in;
		in.open(fn, ios::in | ios::app | ios::binary);
		if (!in) in.open(fn, ios::in | ios::app | ios::binary);
		if (!in) return false;
		stringstream stringStream;
		stringStream << in.rdbuf();
		string binary = stringStream.str();
		in.close();
		this->arch[sz]["data"] = binary;
		return true;
	}

	bool QBZIP::Add(map<string, string> fm)
	{
		lock_guard<std::mutex> alock(this->mutex);
		map<string, string>::iterator it = fm.begin();
		using FUNC = bool(QBZIP::*)(string, string);
		FUNC add1 = &QBZIP::Add;
		for (; it != fm.end(); it++)
		{
			//cout << it->first << " | " << it->second << endl;
			thread th(bind(add1, this, it->first, it->second));
			th.detach();
		}
	}

	bool QBZIP::Save()
	{
		lock_guard<std::mutex> alock(this->mutex);
		string bin = this->arch.zip(cfg->csl);
		this->arch.toc();
		fstream ofs(this->zfps, ios::out | ios::ate | ios::binary);
		if (ofs)
		{
			ofs << bin;
			ofs.close();
		}
	}

	bool QBZIP::UnZip(string dp)
	{
		fstream in(this->zfps, ios::in | ios::app | ios::binary);
		if (in)
		{
			stringstream stringStream;
			stringStream << in.rdbuf();
			string binary = stringStream.str();
			in.close();
		}
		else return false;
		return false;
	}
}
