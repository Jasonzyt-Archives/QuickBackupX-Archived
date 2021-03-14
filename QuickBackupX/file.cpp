// Created by JasonZYT on 2021/03/03
#include "file.h"

using namespace std;

namespace QuickBackupX
{
	Directory::Directory(std::string path)
	{
		this->path = filesystem::path(path);
	}
	Directory::Directory(std::filesystem::path path)
	{
		this->path = path;
	}
	bool Directory::exists()
	{
		return filesystem::exists(this->path);
	}
	bool Directory::ListFiles(std::string dir)
	{
		string tempd = this->path.string() + dir + "/";
		filesystem::directory_iterator dit(tempd);
		Debug{ L_INFO(string("遍历文件夹: ") + tempd); }
		for (auto& it : dit)
		{
			if (it.status().type() == filesystem::file_type::directory)
			{
				this->folder_quan++;
				if (!filesystem::exists(it.path())) continue;
				Debug{ L_INFO(string("- 目录: ") + (it.path().string()) + "/ (" + dir + ")"); }
				if (!filesystem::exists(it.path())) continue;
				this->ListFiles(dir + "/" + it.path().filename().string());
			}
			else if (it.status().type() == filesystem::file_type::regular)
			{
				this->file_quan++;
				if (!filesystem::exists(it.path())) continue;
				this->insert(
				pair<string, string>(
					this->path.string() + dir + "/" + it.path().filename().string(),
					dir + "/" + it.path().filename().string())
				);
				Debug{ L_INFO(string("- 文件: ") + it.path().string() + "(" + dir + ")"); }
			}
		}
		return true;
	}
	bool Directory::ListFiles()
	{
		string tempd = this->path.string();
		filesystem::directory_iterator dit(tempd);
		Debug{ L_INFO(string("遍历文件夹: ") + tempd); }
		for (auto& it : dit)
		{
			if (it.status().type() == filesystem::file_type::directory)
			{
				this->folder_quan++;
				if (!filesystem::exists(it.path())) continue;
				Debug{ L_INFO(string("- 目录: ") + (it.path().string() + "/")); }
				if (!filesystem::exists(it.path())) continue;
				this->ListFiles(it.path().filename().string());
			}
			else if (it.status().type() == filesystem::file_type::regular)
			{
				this->file_quan++;
				if (!filesystem::exists(it.path())) continue;
				this->insert(
					pair<string, string>(
						this->path.string() + it.path().filename().string(),
						it.path().filename().string())
				);
				Debug{ L_INFO(string("- 文件: ") + it.path().string()); }
			}
		}
		return true;
	}
	bool Directory::dirlist()
	{
		try {
			this->ListFiles();
		}
		catch (filesystem::filesystem_error fe) {
			PRERR("FILESYSTEM-ERR: " << to_UTF8(fe.what()));
			L_ERROR(to_UTF8(fe.what()));
			L_ERROR(string("- Path1: ") + fe.path1().string());
			L_ERROR(string("- Path2: ") + fe.path2().string());
			return false;
		}
		catch (...) {
			L_ERROR("出现错误!!!");
			return false;
		}
		return true;
	}
	size_t Directory::dirsize()
	{
		size_t siz = 0;
		try
		{
			this->dirlist();
			map<string, string>::iterator it = this->begin();
			for (; it != this->end(); it++)
			{
				if (!filesystem::exists(it->first)) continue;
				siz += filesystem::file_size(it->first);
			}
		}
		catch (filesystem::filesystem_error fe) {
			PRERR("FILESYSTEM-ERR: " << to_UTF8(fe.what()));
			L_ERROR(to_UTF8(fe.what()));
			L_ERROR(string("- Path1: ") + fe.path1().string());
			L_ERROR(string("- Path2: ") + fe.path2().string());
			return 0;
		}
		catch (...) {
			L_ERROR("出现错误!!!");
			return 0;
		}
		return siz;
	}
	bool Directory::copy_all_to(filesystem::path path)
	{
		try 
		{
			if (filesystem::exists(path))
			{
				PRWARN("目录 " << path << " 已经存在!!!");
				L_WARNING("目录 " + path.string() + " 已经存在!!!");
				Directory dir(path);
				dir.delete_all();
			}
			this->dirlist();
			filesystem::create_directory(path);
			filesystem::copy(this->path, path);
			filesystem::directory_iterator list(this->path);
			L_INFO("遍历文件夹: " + this->path.string());
			for (auto& it : list)
			{
				if (it.status().type() == filesystem::file_type::directory)
				{
					Debug{ L_INFO(string("- 文件夹: ") + it.path().string()); }
					filesystem::copy(it.path(), filesystem::path(path.string() + it.path().filename().string()));
					Debug{ L_INFO(string("已复制 ") + it.path().string() + " 至 " + path.string() + it.path().filename().string()); }
				}
			}
		}
		catch (filesystem::filesystem_error fe) 
		{
			PRERR("FILESYSTEM-ERR: " << to_UTF8(fe.what()));
			L_ERROR(to_UTF8(fe.what()));
			L_ERROR(string("- Path1: ") + fe.path1().string());
			L_ERROR(string("- Path2: ") + fe.path2().string());
			return false;
		}
		catch (...) 
		{
			L_ERROR("出现错误!!!");
			return false;
		}
		return true;
	}
	bool Directory::delete_all()
	{
		try
		{
			Debug{ L_INFO(string("删除目录: ") + this->path.string()); }
			size_t dfc = filesystem::remove_all(this->path);
			Debug{ L_INFO("已删除,共删除了 " + to_string(dfc) + " 个文件"); }
		}
		catch (filesystem::filesystem_error fe)
		{
			PRERR("FILESYSTEM-ERR: " << to_UTF8(fe.what()));
			L_ERROR(to_UTF8(fe.what()));
			L_ERROR(string("- Path1: ") + fe.path1().string());
			L_ERROR(string("- Path2: ") + fe.path2().string());
			return false;
		}
		catch (...)
		{
			L_ERROR("出现错误!!!");
			return false;
		}
		return true;
	}
}