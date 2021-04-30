// Created by Jasonzyt on 2021/04/23
#pragma once
#ifndef QBX_SQLITE_INCLUDED
#define QBX_SQLITE_INCLUDED
#include "pch.h"
#include "backup.h"
#include "func.h"
#include "sqlite3/sqlite3.h"

namespace QuickBackupX
{
	class SQLite
	{
		/*class Exception
		{
		public:
			Exception(const std::string& sql, int errcode, const std::string& errmsg, const std::string& note); 
			Exception(int errcode, const std::string& errmsg, const std::string& note);
			Exception(const std::string& errmsg, const std::string& note);
			Exception(const std::string& sql, int errcode, const std::string& errmsg);
			Exception(int errcode, const std::string& errmsg);
			Exception(const std::string& errmsg);
			inline std::string sql()
			{
				return _Sql;
			}
			inline std::string errmsg()
			{
				return _ErrMsg;
			}
			inline int errcode()
			{
				return _ErrCode;
			}
			inline std::string what()
			{
				return _What;
			}
		private:
			std::string _Sql;
			std::string _ErrMsg;
			std::string _What;
			int _ErrCode;
		};*/
	public:
		typedef std::map<std::string, std::vector<std::string>> SQLResult;
		class BRecResult
		{
		public:
			int res_size;
			std::vector<unsigned int> id;
			std::vector<std::string> time;
			std::vector<std::string> path;
			std::vector<std::string> md5;
			std::vector<size_t> size;
			std::vector<Backup::Executor> exer;
			std::vector<std::string> note;
			Backup::Executor to_Executor(const std::string& str);
		};

		SQLite(const std::string& fp);
		~SQLite();
		
		bool Open(const std::string& fp);
		bool Close();
		bool Table_Exists(const std::string& table);
		bool Table_Create(const std::string& table, const std::string& fields);
		bool Insert(const std::string& table, const std::string& fields, const std::string& content);
		bool Insert(const std::string& table, const std::string& content);
		bool Delete_Row(const std::string& table, const std::string& where_);
		bool Update(const std::string& table, const std::string& set, const std::string& where_);
		bool Field_Exists(const std::string& table, const std::string& field);
		bool Field_Add(const std::string& table, const std::string& field);
		bool Query(const std::string& query, int (*callback)(void*, int, char**, char**) = 0, void* ret = 0);
		BRecResult* Select_All(const std::string& table);
		char* errmsg;
		int errcode;
	private:
		sqlite3* db = nullptr;
	};
}

#endif // !QBX_SQLITE_INCLUDED
