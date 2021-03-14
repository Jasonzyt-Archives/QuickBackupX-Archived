// Created by JasonZYT on 2021/03/03
#pragma once
#ifndef QBX_FILE_INCLUDED
#define QBX_FILE_INCLUDED
#include "pch.h"
#include "logger.h"
#include "config.h"

#define FS_CATCH \
catch (filesystem::filesystem_error fe) {\
	PRERR("FILESYSTEM-ERR: " << to_UTF8(fe.what()));\
	L_ERROR(to_UTF8(fe.what()));\
	L_ERROR(string("- Path1: ") + fe.path1().string());\
	L_ERROR(string("- Path2: ") + fe.path2().string());\
	return 0;\
}\
catch (...) {\
	L_ERROR("出现错误!!!");\
	return 0;\
}

namespace QuickBackupX
{
	typedef std::map<std::string, std::string> FList;
	class Directory : public std::map<std::string, std::string> // 绝对路径 相对路径
	{
	public:
		std::filesystem::path path;
		size_t file_quan = 0;
		size_t folder_quan = 0;
		std::vector<std::filesystem::path> flist;

		Directory(std::string path);
		Directory(std::filesystem::path path);
		bool exists();
		bool dirlist();
		size_t dirsize();
		bool copy_all_to(std::filesystem::path path);
		bool delete_all();
		int files_total();
	private:
		bool ListFiles(std::string dir);
		bool ListFiles();
	};
}

#endif // !QBX_FILE_INCLUDED
