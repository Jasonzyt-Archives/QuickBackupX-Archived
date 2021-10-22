#ifndef BACKUP_H
#define BACKUP_H
#include "pch.h"

namespace QuickBackupX
{
	class Backup
	{
	public:
		struct Executor
		{
			enum Type { All, Unknown, Console, Player, CmdBlk, AutoBak } type;
			std::string pl_name;
			xuid_t pl_xuid;

			Executor(Type tp)
			{
				if (tp == Player) throw std::exception("Must not be Player type");
				else type = tp;
			}
			Executor(std::string pname, xuid_t pxuid) : pl_name(pname), pl_xuid(pxuid) {}

		}			bak_exer;
		std::string bak_uuid;
		std::string bak_md5;
		time_t      bak_tm;
		fs::path    bak_path;


		Backup();
		~Backup();
		bool Make();
		bool Delete();
		bool Resume();
	private:
		std::string getUuid();
	};
}

#endif