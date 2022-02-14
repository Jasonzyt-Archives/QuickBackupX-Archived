#pragma once
#if defined(BDS_LL)

#include <LoggerAPI.h>

#else 

#include <string_view>
#include <iostream>
#include <sstream>
#include <ctime>
#include <Windows.h>
#include <fmt/core.h>
#undef ERROR

enum LOGLVL : char {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3,
    FATAL = 4,
};

enum COLOR : unsigned int {
    BLUE = 9, // Sky blue
    GREEN = 10,
    RED = 12,
    CYAN = 11,
    PINK = 13,
    YELLOW = 14,
    WHITE = 15,

    DK_BLUE = 1,
    DK_GREEN = 2,
    DK_RED = 4,
    DK_CYAN = 3, // Bright blue
    DK_PINK = 5, // Purple
    DK_YELLOW = 6,
    DK_WHITE = 7 // Grey
};

class Logger {

    std::string timeFmt = "%Y-%m-%d %H:%M:%S";
    std::string prefix = "[QBX] ";
    static constexpr std::string_view LVLNAME[5] = { "DEBUG]", "INFO]", "WARN]", "ERROR]", "FATAL]" };

public:

    Logger() = default;
    Logger(const std::string& prefix) {
        this->prefix = '[' + prefix + "] ";
    }

    template <LOGLVL lvl = INFO, COLOR color = WHITE>
    inline void log(const std::string& text) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
        char datebuf[512];
        struct tm _tim;
        {
            auto timnow = time(0);
            localtime_s(&_tim, &timnow);
        }
        auto timsz = strftime(datebuf, 256, ("[" + timeFmt + " ").c_str(), &_tim);
        strcat_s(datebuf, LVLNAME[int(lvl)].data());
        cout << datebuf << prefix << text << endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    }

    template <typename ... Args>
    inline void info(const std::string& str, Args... args) {
        log<INFO, WHITE>(fmt::format(str, args...));
    }
    
    template <typename ... Args>
    inline void warn(const std::string& str, Args... args) {
        log<WARN, YELLOW>(fmt::format(str, args...));
    }

    template <typename ... Args>
    inline void error(const std::string& str, Args... args) {
        log<ERROR, RED>(fmt::format(str, args...));
    }

    template <typename ... Args>
    inline void fatal(const std::string& str, Args... args) {
        log<FATAL, DK_RED>(fmt::format(str, args...));
    }

    template <typename ... Args>
    inline void debug(const std::string& str, Args... args) {
        log<DEBUG, BLUE>(fmt::format(str, args...));
    }

};


#endif