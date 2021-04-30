// Created by Jasonzyt on 2021/04/23
#include "sqlite.h"
#include "logger.h"

using namespace std;

namespace QuickBackupX
{
	/* !!! BUG CODE !!!
	SQLite::Exception::Exception(const string& sql, int errcode, const string& errmsg, const string& note)
	{
		_Sql = sql;
		_ErrCode = errcode;
		_ErrMsg = errmsg;
		_What = note + ": " + errmsg + "(" + to_string(errcode) + "); SQL: " + sql;
	}
	SQLite::Exception::Exception(const string& sql, int errcode, const string& errmsg)
	{
		_Sql = sql;
		_ErrCode = errcode;
		_ErrMsg = errmsg;
		_What = errmsg + "(" + to_string(errcode) + "); SQL: " + sql;
	}
	SQLite::Exception::Exception(int errcode, const string& errmsg, const string& note)
	{
		_ErrCode = errcode;
		_ErrMsg = errmsg;
		_What = note + ": " + errmsg + "(" + to_string(errcode) + ")";
	}
	SQLite::Exception::Exception(int errcode, const string& errmsg)
	{
		_ErrCode = errcode;
		_ErrMsg = errmsg;
		_What = errmsg + "(" + to_string(errcode) + ")";
	}
	SQLite::Exception::Exception(const string& errmsg, const string& note)
	{
		_ErrMsg = errmsg;
		_What = note + ": " + errmsg;
	}
	SQLite::Exception::Exception(const string& errmsg)
	{
		_ErrMsg = errmsg;
		_What = errmsg;
	} !!! BUG CODE !!!
	*/

	SQLite::SQLite(const string& fp)
	{
		db = (sqlite3*)sqlite3_initialize();
		this->Open(fp);
	}
	SQLite::~SQLite()
	{
		sqlite3_close(db);
		sqlite3_free(db);
	}
	bool SQLite::Open(const string& fp)
	{
		if (sqlite3_open(fp.c_str(), &db))
		{
			ostringstream ostr;
			ostr << "SQLite ERROR(" << to_string(sqlite3_errcode(db)) << ")! " << fp << " 打开失败!!!" << endl
				 << "- MSG: " << errmsg;
			L_ERROR(ostr.str());
			PRERR(ostr.str());
			sqlite3_close(db);
			return false;
		}
		return true;
	}
	bool SQLite::Close()
	{
		if (sqlite3_close(db)) return false;
		return true;
	}
	bool SQLite::Query(const string& query, int (*callback)(void*, int, char**, char**), void* ret)
	{
		if (sqlite3_exec(db, query.c_str(), callback, 0, &errmsg))
		{
			ostringstream ostr;
			ostr << "SQLite ERROR(" << to_string(sqlite3_errcode(db)) << ")! SQL执行失败!!!" << endl
				<< "- SQL: " << query << endl << "- MSG: " << errmsg;
			L_ERROR(ostr.str());
			PRERR(ostr.str());
			return false;
		}
		return true;
	}
	bool SQLite::Table_Create(const string& table, const string& fields)
	{
		string query = "CREATE TABLE IF NOT EXISTS " + table + "(" + fields + ");";
		return this->Query(query);
	}
	bool SQLite::Table_Exists(const string& table)
	{
		string query = "select count(*) from sqlite_master where type='table' and name'" + table + "';";
		int tables = 0;
		int qres = this->Query(query, 
			[](void* han, int ret, char** src, char** dst)->int {
				if (1 == ret)
				{
					int texist = atoi(*(src));
					if (han != nullptr)
					{
						int* res = (int*)han;
						*res = texist;
					}
				}
				return 0;
			}, 
			(void*)tables
		); 
		return tables > 0;
	}
	bool SQLite::Insert(const string& table, const string& fields, const string& content)
	{
		string query = "insert into `" + table + "` (" + fields + ") values(" + content + ")";
		return this->Query(query);
	}
	bool SQLite::Insert(const string& table, const string& content)
	{
		string query = "insert into `" + table + "`  values(" + content + ")";
		return this->Query(query);
	}
	bool SQLite::Delete_Row(const string& table, const string& where_)
	{
		string query = "DELETE FROM `" + table + "` WHERE " + where_ + ";";
		return this->Query(query);
	}
	bool SQLite::Update(const string& table, const string& set, const string& where_)
	{
		string query = "UPDATE `" + table + "` SET " + set + " WHERE " + where_ + ";";
		return this->Query(query);
	}
	SQLite::BRecResult* SQLite::Select_All(const string& table)
	{
		BRecResult* res = new BRecResult;
		char** src;
		int rows, cols;
		sqlite3_stmt* stmt = NULL;
		const char* tail;
		string query = "SELECT * FROM `" + table + "`;";
		if (!sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, &tail)) {
			int siz = 0;
			while (sqlite3_step(stmt) == SQLITE_ROW) {
				res->id  .push_back(sqlite3_column_int(stmt, 0));
				res->time.push_back((const char*)sqlite3_column_text(stmt, 1));
				res->path.push_back((const char*)sqlite3_column_text(stmt, 2));
				res->md5 .push_back((const char*)sqlite3_column_text(stmt, 3));
				res->size.push_back(sqlite3_column_int64(stmt, 4));
				res->exer.push_back(res->to_Executor((const char*)sqlite3_column_text(stmt, 5)));
				res->note.push_back((const char*)sqlite3_column_text(stmt, 6));
				siz++;
			}
			res->res_size = siz;
		}
		sqlite3_finalize(stmt);
		return res;
	}


	Backup::Executor SQLite::BRecResult::to_Executor(const string& str)
	{
		Backup::Executor rv;
		vector<string> vec = split(str, ';');
		switch (atoi(vec[0].c_str()))
		{
		case 0: break;
		case 1: break;
		case 2:
			rv.type = Console_Type;
			break;
		case 3:
			rv.type = Player_Type;
			rv.pname = vec[1];
			rv.pxuid = vec[2];
			break;
		case 4:
			rv.type = Block_Type;
			rv.cbpos = vec[1];
			break;
		case 5:
			rv.type = AutoBak_Type;
			break;
		}
		return rv;
	}
}
