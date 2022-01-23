#include "SyncCopy.h"

void SyncCopy::directories(fs::path src, std::string dst) {
	if (fs::exists(src)) {
		for (auto& it : fs::directory_iterator(src)) {
			if (it.is_directory()) {
				directories(it.path(), dst + it.path().filename().string() + '/');
			}
			else {
				list.push({ it.path(),dst + it.path().filename().string() });
			}
		}
	}
}

SyncCopy::SyncCopy(std::string src, std::string dst, int mfpt) : maxFiles(mfpt) {
	directories(src, dst);
}

void SyncCopy::tick() {
	int cnt = 0;
	while (!list.empty() && cnt < maxFiles) {
		auto f = list.front();
		list.pop();
		fs::copy_file(f.src, f.dst);
	}
}

bool SyncCopy::done() {
	return list.empty();
}