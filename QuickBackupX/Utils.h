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