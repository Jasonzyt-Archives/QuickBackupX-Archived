#pragma once
#include "LangPack.h"

class QuickBackupX {

public:

	std::unique_ptr<Config> cfg;
	std::unique_ptr<LangPack> lpk; 

	QuickBackupX();

};