// Created by JasonZYT on 2021/02/06
#pragma once
#ifndef QBX_LOGGER_INCLUDED
#define QBX_LOGGER_INCLUDED
#include "pch.h"
#include "func.h"

#define PR(...)      std::cout << "[" << QBX::getTime() << " INFO][QuickBackupX] " << __VA_ARGS__ << std::endl
#define PRDEBUG(...) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);std::cout << "[" << QBX::getTime() << " DEBUG][QuickBackupX] " << __VA_ARGS__ << std::endl;SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
#define PRWARN(...)  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);std::cout << "[" << QBX::getTime() << " WARN][QuickBackupX] " << __VA_ARGS__ << std::endl;SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN)
#define PRERR(...)   SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);std::cout << "[" << QBX::getTime() << " ERROR][QuickBackupX] " << __VA_ARGS__ << std::endl;SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN)

#define L_INFO(...)    QBX::log->LogInfo(__VA_ARGS__)
#define L_WARNING(...) QBX::log->LogWarning(__VA_ARGS__, __FUNCTION__, __LINE__, __FILE__)
#define L_ERROR(...)   QBX::log->LogError(__VA_ARGS__, __FUNCTION__, __LINE__, __FILE__)
#define L_DEBUG(...)   QBX::log->LogDebugInfo(__VA_ARGS__, __FUNCTION__, __LINE__, __FILE__)

namespace QuickBackupX
{
	class Logger
	{
	public:
		~Logger();
		bool Open();
		bool Start();
		bool LogInfo(std::string evt);
		bool LogWarning(std::string evt, std::string fun = "", int line = -1, std::string file = "");
		bool LogError(std::string evt, std::string fun = "", int line = -1, std::string file = "");
		bool LogDebugInfo(std::string evt, std::string fun = "", int line = -1, std::string file = "");
		bool Log(std::string tag, std::string tags, std::string evt, std::string fun = "", int line = -1, std::string file = "");
		std::ofstream fp;
		bool status = false;
	};
}

#endif // !QBX_LOGGER_INCLUDED
