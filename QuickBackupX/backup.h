// Created by JasonZYT on 2021/02/07
#pragma once
#ifndef QBX_BACKUP_INCLUDED
#define QBX_BACKUP_INCLUDED
#include "pch.h"
#include "func.h"
#include "file.h"
#include "json/json.h"
#include "zip/zip.h"
#include "qbzip.h"
//#include "zip/unzip.h"

#define Console_Type QuickBackupX::Backup::Executor::Type::Console
#define Player_Type  QuickBackupX::Backup::Executor::Type::Player
#define Block_Type   QuickBackupX::Backup::Executor::Type::Block
#define AutoBak_Type QuickBackupX::Backup::Executor::Type::AutoBackup
#define Unknown_Type QuickBackupX::Backup::Executor::Type::Unknown

namespace QuickBackupX
{
	class Backup
	{
	public:
		class Executor
		{
		public:
			enum class Type : int
			{
				Unknown, Console, Player, Block, AutoBackup
			};
			enum class CBMode : int
			{
				Pulse, Chain, Repeating
			};
			/// 执行者类型
			Type type = Type::Unknown;
			/// 玩家名
			std::string pname = "";
			/// 玩家Xuid
			std::string pxuid = "";
			/// 命令方块位置
			std::string cbpos = "";
			/// 命令方块模式
			CBMode cbmode = CBMode::Pulse;
			/// 命令方块是否为有条件的(未使用)
			bool cbcdit = false;
			/// <summary>
			/// 将Executor类型转换为字符串 格式: [Unknown]/[Console]/[Player]pname/[Block]cbpos
			/// </summary>
			/// <returns>std::string 结果</returns>
			std::string to_string();
			/// <summary>
			/// 将Executor::Type转换为字符串
			/// </summary>
			/// <returns>std::string 结果</returns>
			std::string type_string();
			/// <summary>
			/// 将cbpos转换为直观的字符串 格式: 维度ID:X,Y,Z -> [维度中文]X,Y,Z
			/// </summary>
			/// <returns>std::string 结果</returns>
			std::string format_pos();
			/// 是否在回档后删除源备份
			bool delsrcbak = false;
			/// 是否在回档后删除原存档 (true:移动到[RESUMEDIR],false:永久删除)
			bool delsrclevel = false;
		};

		/// 构造函数
		Backup();
		/// 析构函数
		~Backup();
		/// <summary>
		/// 创建一个备份
		/// </summary>
		/// <param name="exer">执行者</param>
		/// <returns>bool 备份成功与否</returns>
		bool Make  (Executor exer);
		/// <summary>
		/// 删除一个备份
		/// </summary>
		/// <param name="exer">执行者</param>
		/// <returns>bool 是否删除</returns>
		bool Delete(Executor exer);
		/// <summary>
		/// 从该备份回档
		/// </summary>
		/// <param name="exer">执行者</param>
		/// <returns>bool 回档执行成功与否(后续交由外部进程处理)</returns>
		bool Resume(Executor exer);

		/// 零时文件列表
		std::map<std::string,std::string> flist;
		/// 存档路径 -> 构造函数
		std::filesystem::path lpath;
		/// 备份路径
		std::filesystem::path path;
		/// 备份文件的MD5值
		std::string md5 = "";
		/// 备份的时间
		std::string time = "";
		/// 备份执行者
		Executor exer;
		/// 备份存档大小
		size_t size = -1;
		/// 备份序号
		int onum = -1;
	private:
		/// <summary>
		/// 检查备份权限
		/// </summary>
		/// <returns>true:有权限 false:无权限</returns>
		bool CheckBackupPermission();
		/// <summary>
		/// 检查删除备份权限
		/// </summary>
		/// <param name="exer">执行者</param>
		/// <returns>true:有权限 false:无权限</returns>
		bool CheckDeletePermission(Executor exer);
		/// <summary>
		/// 检查回档权限
		/// </summary>
		/// <param name="exer">执行者</param>
		/// <returns>true:有权限 false:无权限</returns>
		bool CheckResumePermission(Executor exer);
		/// <summary>
		/// 创建备份之压缩
		/// </summary>
		/// <returns>0为成功 其他为异常(配合ZipRetCheck函数使用)</returns>
		ZRESULT Create();
		/// <summary>
		/// 备份回档之解压
		/// </summary>
		/// <returns>0为成功 其他为异常(配合ZipRetCheck函数使用)</returns>
		ZRESULT Unzip();
		/// <summary>
		/// 获取存档大小
		/// </summary>
		/// <returns>unsigned long long 字节数</returns>
		size_t getLevelSize();
		/// <summary>
		/// 遍历备份文件夹
		/// </summary>
		/// <param name="path">相对备份目录路径</param>
		/// <returns>bool 是否成功(数据储存在this->flist)</returns>
		bool IterTempDir(std::string path = "");
		/// <summary>
		/// 复制存档至临时文件夹(直接压缩无法访问)
		/// </summary>
		/// <returns>bool 成功与否(出现错误大概率直接throw)</returns>
		bool CopyLevelToTempDir();
		/// <summary>
		/// 删除临时文件夹
		/// </summary>
		/// <returns>bool 成功与否(出现错误大概率直接throw)</returns>
		bool DeleteTempDir();
	};
}

#endif // !QBX_BACKUP_INCLUDED
