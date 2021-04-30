// Created by JasonZYT on 2021/03/08
#pragma once
#ifndef QBX_ZIP_INCLUDED
#define QBX_ZIP_INCLUDED
#include "pch.h"
#include "file.h"

#pragma region BUNDLE_TYPEDEF
enum
{
	MZ_ZIP_MAX_IO_BUF_SIZE = 64 * 1024,
	MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE = 260,
	MZ_ZIP_MAX_ARCHIVE_FILE_COMMENT_SIZE = 256
};
typedef unsigned char mz_uint8;
typedef signed short mz_int16;
typedef unsigned short mz_uint16;
typedef unsigned int mz_uint32;
typedef unsigned int mz_uint;
typedef long long mz_int64;
typedef unsigned long long mz_uint64;
typedef int mz_bool;
typedef void* (*mz_alloc_func)(void* opaque, size_t items, size_t size);
typedef void (*mz_free_func)(void* opaque, void* address);
typedef void* (*mz_realloc_func)(void* opaque, void* address, size_t items, size_t size);
typedef struct
{
	mz_uint32 m_file_index;
	mz_uint32 m_central_dir_ofs;
	mz_uint16 m_version_made_by;
	mz_uint16 m_version_needed;
	mz_uint16 m_bit_flag;
	mz_uint16 m_method;
#ifndef MINIZ_NO_TIME
	time_t m_time;
#endif
	mz_uint32 m_crc32;
	mz_uint64 m_comp_size;
	mz_uint64 m_uncomp_size;
	mz_uint16 m_internal_attr;
	mz_uint32 m_external_attr;
	mz_uint64 m_local_header_ofs;
	mz_uint32 m_comment_size;
	char m_filename[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE];
	char m_comment[MZ_ZIP_MAX_ARCHIVE_FILE_COMMENT_SIZE];
} mz_zip_archive_file_stat;

typedef size_t(*mz_file_read_func)(void* pOpaque, mz_uint64 file_ofs, void* pBuf, size_t n);
typedef size_t(*mz_file_write_func)(void* pOpaque, mz_uint64 file_ofs, const void* pBuf, size_t n);

struct mz_zip_internal_state_tag;
typedef struct mz_zip_internal_state_tag mz_zip_internal_state;

typedef enum
{
	MZ_ZIP_MODE_INVALID = 0,
	MZ_ZIP_MODE_READING = 1,
	MZ_ZIP_MODE_WRITING = 2,
	MZ_ZIP_MODE_WRITING_HAS_BEEN_FINALIZED = 3
} mz_zip_mode;

typedef struct mz_zip_archive_tag
{
	mz_uint64 m_archive_size;
	mz_uint64 m_central_directory_file_ofs;
	mz_uint m_total_files;
	mz_zip_mode m_zip_mode;

	mz_uint m_file_offset_alignment;

	mz_alloc_func m_pAlloc;
	mz_free_func m_pFree;
	mz_realloc_func m_pRealloc;
	void* m_pAlloc_opaque;

	mz_file_read_func m_pRead;
	mz_file_write_func m_pWrite;
	void* m_pIO_opaque;

	mz_zip_internal_state* m_pState;

} mz_zip_archive;

typedef enum
{
	MZ_ZIP_FLAG_CASE_SENSITIVE = 0x0100,
	MZ_ZIP_FLAG_IGNORE_PATH = 0x0200,
	MZ_ZIP_FLAG_COMPRESSED_DATA = 0x0400,
	MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY = 0x0800
} mz_zip_flags;
#pragma endregion

namespace QuickBackupX
{
	class QBZIP
	{
	public:
		QBZIP(std::string zfp);
		QBZIP(std::filesystem::path zfp);
		QBZIP(const char* zfp);
		~QBZIP();
		bool Init();
		bool Add(const std::string& zp, const std::string& content);
		bool AddFile(const std::string& fn, const std::string& zp);
		bool AddFList(std::map<std::string, std::string>& fm);
		bool Save();
		bool Read();
		bool UnZip(std::string dp);
	private:
		std::string ReadFile(const std::string& fn, const size_t len = 0);
		bool Write();
		bool _AddInternal(const std::string& zp, const std::string& content);
		bool _AddFileInternal(const std::string& fn, const std::string& zp);
		bool _AddFListInternal(std::map<std::string, std::string>& fm);
		std::mutex mutex;

		std::string zipbin;
		mz_zip_archive zip_archive;
		mz_bool status;
		bool can_write = false;
		bool no_error = true;

		std::fstream file;
		std::string zfps;
		std::filesystem::path zfp;
	};
}

#endif // !QBX_ZIP_INCLUDED
