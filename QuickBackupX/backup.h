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
			/// ִ��������
			Type type = Type::Unknown;
			/// �����
			std::string pname = "";
			/// ���Xuid
			std::string pxuid = "";
			/// �����λ��
			std::string cbpos = "";
			/// �����ģʽ
			CBMode cbmode = CBMode::Pulse;
			/// ������Ƿ�Ϊ��������(δʹ��)
			bool cbcdit = false;
			/// <summary>
			/// ��Executor����ת��Ϊ�ַ��� ��ʽ: [Unknown]/[Console]/[Player]pname/[Block]cbpos
			/// </summary>
			/// <returns>std::string ���</returns>
			std::string to_string();
			/// <summary>
			/// ��Executor::Typeת��Ϊ�ַ���
			/// </summary>
			/// <returns>std::string ���</returns>
			std::string type_string();
			/// <summary>
			/// ��cbposת��Ϊֱ�۵��ַ��� ��ʽ: ά��ID:X,Y,Z -> [ά������]X,Y,Z
			/// </summary>
			/// <returns>std::string ���</returns>
			std::string format_pos();
			/// �Ƿ��ڻص���ɾ��Դ����
			bool delsrcbak = false;
			/// �Ƿ��ڻص���ɾ��ԭ�浵 (true:�ƶ���[RESUMEDIR],false:����ɾ��)
			bool delsrclevel = false;
		};

		/// ���캯��
		Backup();
		/// ��������
		~Backup();
		/// <summary>
		/// ����һ������
		/// </summary>
		/// <param name="exer">ִ����</param>
		/// <returns>bool ���ݳɹ����</returns>
		bool Make  (Executor exer);
		/// <summary>
		/// ɾ��һ������
		/// </summary>
		/// <param name="exer">ִ����</param>
		/// <returns>bool �Ƿ�ɾ��</returns>
		bool Delete(Executor exer);
		/// <summary>
		/// �Ӹñ��ݻص�
		/// </summary>
		/// <param name="exer">ִ����</param>
		/// <returns>bool �ص�ִ�гɹ����(���������ⲿ���̴���)</returns>
		bool Resume(Executor exer);

		/// ��ʱ�ļ��б�
		std::map<std::string,std::string> flist;
		/// �浵·�� -> ���캯��
		std::filesystem::path lpath;
		/// ����·��
		std::filesystem::path path;
		/// �����ļ���MD5ֵ
		std::string md5 = "";
		/// ���ݵ�ʱ��
		std::string time = "";
		/// ����ִ����
		Executor exer;
		/// ���ݴ浵��С
		size_t size = -1;
		/// �������
		int onum = -1;
	private:
		/// <summary>
		/// ��鱸��Ȩ��
		/// </summary>
		/// <returns>true:��Ȩ�� false:��Ȩ��</returns>
		bool CheckBackupPermission();
		/// <summary>
		/// ���ɾ������Ȩ��
		/// </summary>
		/// <param name="exer">ִ����</param>
		/// <returns>true:��Ȩ�� false:��Ȩ��</returns>
		bool CheckDeletePermission(Executor exer);
		/// <summary>
		/// ���ص�Ȩ��
		/// </summary>
		/// <param name="exer">ִ����</param>
		/// <returns>true:��Ȩ�� false:��Ȩ��</returns>
		bool CheckResumePermission(Executor exer);
		/// <summary>
		/// ��������֮ѹ��
		/// </summary>
		/// <returns>0Ϊ�ɹ� ����Ϊ�쳣(���ZipRetCheck����ʹ��)</returns>
		ZRESULT Create();
		/// <summary>
		/// ���ݻص�֮��ѹ
		/// </summary>
		/// <returns>0Ϊ�ɹ� ����Ϊ�쳣(���ZipRetCheck����ʹ��)</returns>
		ZRESULT Unzip();
		/// <summary>
		/// ��ȡ�浵��С
		/// </summary>
		/// <returns>unsigned long long �ֽ���</returns>
		size_t getLevelSize();
		/// <summary>
		/// ���������ļ���
		/// </summary>
		/// <param name="path">��Ա���Ŀ¼·��</param>
		/// <returns>bool �Ƿ�ɹ�(���ݴ�����this->flist)</returns>
		bool IterTempDir(std::string path = "");
		/// <summary>
		/// ���ƴ浵����ʱ�ļ���(ֱ��ѹ���޷�����)
		/// </summary>
		/// <returns>bool �ɹ����(���ִ�������ֱ��throw)</returns>
		bool CopyLevelToTempDir();
		/// <summary>
		/// ɾ����ʱ�ļ���
		/// </summary>
		/// <returns>bool �ɹ����(���ִ�������ֱ��throw)</returns>
		bool DeleteTempDir();
	};
}

#endif // !QBX_BACKUP_INCLUDED
