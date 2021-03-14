// Created by JasonZYT on 2021/03/08
#pragma once
#ifndef QBX_ZIP_INCLUDED
#define QBX_ZIP_INCLUDED
#include "pch.h"
#include "file.h"
#include "bundle/bundle.h"

namespace QuickBackupX
{
	class QBZIP
	{
	public:
		QBZIP(std::string zfp);
		QBZIP(std::filesystem::path zfp);
		~QBZIP();
		bool Add(std::string fn, std::string zp);
		bool Add(std::map<std::string, std::string> fm);
		bool Save();
		bool UnZip(std::string dp);
	private:
		std::mutex mutex;
		bundle::archive arch;
		std::fstream file;
		std::string zfps;
		std::filesystem::path zfp;
	};
}

#endif // !QBX_ZIP_INCLUDED
