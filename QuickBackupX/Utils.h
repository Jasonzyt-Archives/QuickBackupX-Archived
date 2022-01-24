#pragma once
#include "pch.h"

struct DiskUsage {
	uint32_t type;
	size_t free;   // Free bytes
	size_t avfree; // Available free bytes
	size_t total;  // Total bytes
};

inline DiskUsage getDiskUsage(char s) {
	size_t avfree = 0; // Available Free Bytes
	size_t total = 0;  // Total Bytes
	size_t free = 0;   // Free Bytes
	const char dname[] = { s, ':', '\0' };
	auto dtype = GetDriveTypeA(dname);
	auto res = GetDiskFreeSpaceExA(dname,
		(ULARGE_INTEGER*)&avfree,
		(ULARGE_INTEGER*)&total,
		(ULARGE_INTEGER*)&free);
	if (res) {
		return { dtype, free, avfree, total };
	}
	return { UINT_MAX };
}

template<typename ... Args>
inline std::string format(const std::string& format, Args... args)
{
	int size = snprintf(nullptr, 0, format.c_str(), args...) + 1;
	if (size <= 0)
		return "";
	std::unique_ptr<char[]> buf(new char[size]);
	snprintf(buf.get(), size, format.c_str(), args ...);
	return std::string(buf.get(), buf.get() + size - 1);
}
